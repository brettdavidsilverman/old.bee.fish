class RemoteStorage
{
   url = "https://bee.fish";
   
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
      if (value === null)
         return this.removeItem(key);
         
      
      value = btoa(String(value));
      
      var body =
         JSON.stringify(
            {
               method: "setItem",
               key:    key, 
               value:  value
            }
         );
      console.log("set: " + value.length);
      var xhr = new XMLHttpRequest();
      xhr.open('POST', this.url, false);
      xhr.withCredentials = true;
      xhr.send(body);
      var json;
      try
      {
         json = JSON.parse(xhr.responseText);
      }
      catch (ex)
      {
         throw "Invalid JSON.\n" 
            + xhr.status
            + "\n"
            + xhr.responseText;
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
      
      console.log("get: " + xhr.responseText);
      
      var json = JSON.parse(xhr.responseText);
      
      if ( json.key != key ||
           json.response != "ok" )
         throw xhr.responseText;
         
      if (json.value == null)
         return null;
      return atob(json.value);
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
   
   clear()
   {
      var xhr = new XMLHttpRequest();
      xhr.open('POST', this.url, false);
      xhr.withCredentials = true;
      xhr.send(
         JSON.stringify(
            {
               method:"clear"
            }
         )
            
      );

      var json = JSON.parse(xhr.responseText);
      if ( json.response != "ok" )
         throw xhr.responseText;
      return json.response;
   }
   
}

var remoteStorage =
   new RemoteStorage();
   