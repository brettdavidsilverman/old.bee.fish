class RemoteStorage
{
   url = "https://bee.fish/storage/";
   
   constructor(url)
   {
      if (url)
         this.url = url;
      else
      {
         var hostname =
             window.location.hostname;
         var port =
            window.location.port;
            
         this.url = "https://" + hostname
         
         if (port)
            this.url += ":" + port;
            
         this.url += window.location.pathname;
         
      }
      
   }


   setItem(key, value)
   {
      var body =
         JSON.stringify(
            {
               method: "setItem",
               key:    key, 
               value:  value == null ?
                          null :
                          String(value)
            }
         );

      var xhr = new XMLHttpRequest();
      xhr.open('POST', this.url, false);
      xhr.withCredentials = true;
      xhr.send(body);
      var json;
      try
      {
         json = JSON.parse(xhr.response);
      }
      catch (ex)
      {
         throw "Invalid response\n" + xhr.response;
      }
      
      if ( json.key != key ||
           json.response != "ok" )
         throw xhr.responseText;
         
      return json.response;
   }
   
   setItemAsync(key, value)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         JSON.stringify(
            {
               "method": "setItem",
               "key": key,
               "value": value == null ?
                           null :
                           String(value)
            }
         );
         
      var promise = fetch(this.url, params)
         .then(function(response) {
            return response.json();
         })
         .then(function(json) {
            if (json != "ok")
               throw json;
            return json;
         })
         .catch(function(error) {
            throw ("Request failed " + error);
         });
         
      return promise;
   }

   getItem(key)
   {
      var xhr = new XMLHttpRequest();
      xhr.open('POST', this.url, false);
      xhr.withCredentials = true;
      xhr.send(
         JSON.stringify(
            {
               method:"getItem",
               key: key
            }
         )
      );
      var json = JSON.parse(xhr.responseText);
      if ( json.key != key ||
           json.response != "ok" )
         throw xhr.responseText;
      return json.value;
   }
   
   getItemAsync(key)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         JSON.stringify(
            {
               "method": "getItem",
               "key": key
            }
         );
      var promise = fetch(this.url, params)
         .then(function(response) {
            return response.json();
         })
         .then(function(json) {
            return json;
         })
         .catch(function(error) {
            throw "Request failed " + error;
         });
      
      return promise;
   }

   removeItem(key)
   {
      var xhr = new XMLHttpRequest();
      xhr.open('POST', this.url, false);
      xhr.withCredentials = true;
      xhr.send(
         JSON.stringify(
            {
               method:"removeItem",
               key: key
            }
         )
            
      );

      var json = JSON.parse(xhr.responseText);
      if ( json.key != key ||
           json.response != "ok" )
         throw xhr.responseText;
      return json.response;
   }
   
   removeItemAsync(key)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         JSON.stringify(
            {
               "method": "removeItem",
               "key": key
            }
         );
      var promise = fetch(this.url, params)
         .then(function(response) {
            return response.json();
         })
         .then(function(json) {
            return json;
         })
         .catch(function(error) {
            throw "Request failed " + error;
         });
      
      return promise;
   }
}

var remoteStorage =
   new RemoteStorage();