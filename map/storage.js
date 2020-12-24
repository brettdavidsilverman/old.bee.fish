class RemoteStorage
{
   url = "https://bee.fish/map/";
   
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
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         JSON.stringify(
            {
               "method": "setItem",
               "key": key,
               "value": value
            }
         );
    
      return fetch(this.url, params)
         .then(function(response) {
            return response.text();
         })
         .then(function(text) {
            var json = JSON.parse(text);
            if (json.response != "ok")
               throw json.response;
            return json.response;
         })
         .catch(function(error) {
            throw ("Request failed " + error);
         });
   }
   
   getItem(key)
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
         
      return fetch(this.url, params)
         .then(function(response) {
            return response.text();
         })
         .then(function(text) {
            var json = JSON.parse(text);
            var base64 = json.response.value;
            return atob(base64);
         })
         .catch(function(error) {
            throw "Request failed " + error;
         });
   }
   
}

var remoteStorage =
   new RemoteStorage();