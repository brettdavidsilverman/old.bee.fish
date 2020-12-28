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
         (
            {
               method: "setItem",
               key:    key, 
               value:  value == null ?
                          null :
                          String(value)
            }
         ).toString();

      var xhr = new XMLHttpRequest();
      xhr.open('POST', this.url, false);
      xhr.withCredentials = true;
      xhr.send(body);
      var response = JSON.parse(xhr.response);
      return response;
   }
   
   setItemAsync(key, value)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         (
            {
               "method": "setItem",
               "key": key,
               "value": value == null ?
                           null :
                           String(value)
            }
         ).toString();
         
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
         (
            {
               method:"getItem",
               key: key
            }
         ).toString()
      );
      var response = JSON.parse(xhr.responseText);
      return response;
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
         (
            {
               method:"removeItem",
               key: key
            }
         ).toString()
      );

      var response = JSON.parse(xhr.responseText);
      return response;
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