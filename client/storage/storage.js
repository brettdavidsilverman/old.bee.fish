class RemoteStorage
{

   url = "https://bee.fish";
   
   usePromise = true;
   
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
               method: "setItem",
               key,
               value: JSON.stringify(value)
            },
            null,
            "   "
         );
         
      var promise = fetch(this.url, params)
         /*
         .then(
            response => response.text()
         )
         .then(
            text => alert(text)
         );
         */
         .then(
            response =>
               response.json()
         )
         .then(
            json => {
               if (json.response != "ok")
                  throw json;
               return json.key;
            }
         )
         .catch(
            error => {
               throw new Error("1Request failed " + error);
            }
         );
   
      return promise;
   }
   
   getItem(key)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         JSON.stringify(
            {
               method: "getItem",
               key
            }
         );
      var promise = fetch(this.url, params)
         .then(
            response => response.json()
         )
         .then(
            (json) => {
               if (json.response != "ok")
                  throw json;
               return JSON.parse(json.value);
            }
         )
         .catch(
            (error) => {
               throw new Error("2Request failed " + error.stack);
            }
         );
      
      return promise;
   }

   removeItem(key)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         JSON.stringify(
            {
               method: "removeItem",
               key
            }
         );
      var promise = fetch(this.url, params)
         .then(
            response =>
               response.json()
         )
         .then(
            json => {
               if (json.response != "ok")
                  throw json;
               return json.key;
            }
         )
         .catch(
            error => {
               throw new Error("3Request failed " + error);
            }
         );
      
      return promise;
   }

   clear()
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body =
         JSON.stringify(
            {
               method: "clear"
            }
         );
      var promise = fetch(this.url, params)
         .then(function(response) {
            return response.json();
         })
         .then(function(json) {
            if (json.response != "ok")
               throw json;
            return json;
         })
         .catch(function(error) {
            throw new Error("4Request failed " + error);
         });
      
      return promise;
   }
}

var remoteStorage =
   new RemoteStorage();
   
class Storage
{
   _storage = undefined;
   _usePromise = undefined;
   
   constructor(storage)
   {
      this._storage = storage;
      this._usePromise =
         (storage.usePromise == true);
   }
   
   setItem(key, value)
   {
      if (this._usePromise)
      {
         var promise =
            this._storage.setItem(key, value);
         return promise;
      }
      else
      {
         try
         {
            this._storage.setItem(key, value);
            return Promise.resolve(
               key
            );
         }
         catch(error)
         {
            return Promise.reject(error);
         }
  
      }
   }
   
   getItem(key)
   {
      if (this._usePromise)
         return this._storage.getItem(key);
      else
      {
         try
         {
            var value =
               this._storage.getItem(key);
            return Promise.resolve(value);
         }
         catch(error)
         {
            return Promise.reject(error);
         }
      }
      
   }
   
   removeItem(key)
   {
      if (this._usePromise)
         return this._storage.removeItem(key)
            .then(
               (result) => key
            );
      else
      {
         try
         {
            this._storage.removeItem(key);
            return Promise.resolve(
               key
            );
         }
         catch(error)
         {
            return Promise.reject(error);
         }
      }
   }
   
   clear()
   {
      if (this._usePromise)
         return this._storage.clear();
      else
      {
         try
         {
            this._storage.clear();
            return Promise.resolve(
               {response: "ok"}
            );
         }
         catch(error)
         {
            return Promise.reject(error);
         }
      }
      
   }
   
}

var storage = new Storage(remoteStorage);

   