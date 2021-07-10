class RemoteStorage
{

   url = document.location.origin;
   
   local = true;
   
   constructor(input) {
      Object.assign(this, input);
   }
   
   setItem(key, value)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      
      var id = undefined;
      if (key instanceof Id) {
         id = key.key;
         key = undefined;
      }
      
      params.body =
         JSON.stringify(
            {
               method: "setItem",
               key,
               id,
               value
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
               return json.id ?
                  Id.fromKey(json.id) : json.key;
            }
         )
         .catch(
            error => {
               throw new Error(
                  "Set item request failed: " + 
                  error
               );
            }
         );
   
      return promise;
   }
   
   getItem(key)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      
      var id = undefined;
      if (key instanceof Id) {
         id = key.key;
         key = undefined;
      }
      
      params.body =
         JSON.stringify(
            {
               method: "getItem",
               key,
               id
            }
         );
      var promise = fetch(this.url, params)
         /*
         .then(
            response => response.text()
         )
         .then(
            text => alert(text)
         )
         */
         .then(
            response => response.json()
         )
         .then(
            (json) => {
               if (json.response != "ok")
                  throw json;
               return json.value;
            }
         )
         .catch(
            (error) => {
               throw new Error(
                  "Get item request failed: " + 
                  error
               );
            }
         );
      
      return promise;
   }

   removeItem(key)
   {
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      
      var id = undefined;
      if (key instanceof Id) {
         id = key.key;
         key = undefined;
      }
      
      params.body =
         JSON.stringify(
            {
               method: "removeItem",
               key,
               id
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
               return json.id ?
                  Id.fromKey(json.id) : json.key;
            }
         )
         .catch(
            error => {
               throw new Error(
                  "Remove item request failed: " + 
                  error
               );
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
            throw new Error(
                  "Clear request failed: " + 
                  error
               );
         });
      
      return promise;
   }
}

var remoteStorage =
   new RemoteStorage();
   
class Storage
{
   _storage = undefined;
   _local = undefined;
   
   constructor(storage)
   {
      this._storage = storage;
      this._local =
         (storage.local == true);
   }
   
   setItem(key, value)
   {
      if (this._local)
      {
         var promise =
            this._storage.setItem(key, value);
         return promise;
      }
      else
      {
         try
         {
            var newKey;
            if (key instanceof Id) {
               id = key;
               newKey = id.key;
            }
            else
               newKey = key;
               
            this._storage.setItem(newKey, value);
            
            return Promise.resolve(
               newKey
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
      if (this._local)
         return this._storage.getItem(key);
      else
      {
         try
         {
            var newKey;
            if (key instanceof Id) {
               id = key;
               newKey = id.key;
            }
            else
               newKey = key;
               
            var value =
               this._storage.getItem(newKey);
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
      if (this._local)
         return this._storage.removeItem(key)
            .then(
               (result) => key
            );
      else
      {
         try
         {
            var newKey;
            if (key instanceof Id) {
               id = key.key;
               newKey = id.key;
            }
            else
               newKey = key;
            this._storage.removeItem(newKey);
            return Promise.resolve(
               newKey
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
      if (this._local)
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

