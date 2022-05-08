class RemoteStorage
{
   constructor(input) {
      Object.assign(this, input);
      if (this.url == undefined)
         this.url = document.location;
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
         .then(
            function(response) {
               if (response.status != 200)
                  throw "Invalid response: " + response.statusText;
               return response.json()
            }
         )
         .then(
            function(json) {
               
               if (json.response != "ok")
                  throw json;
               
               if (json.id) {
                  var id = Id.fromKey(json.id);
                  return id;
               }

               return json.key;
            }
         )
         .catch(
            function(error)  {
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
         .then(
            function(response) {

               if (response.status != 200)
                  throw "Invalid response: " + response.statusText;
               
               return response.text();
            }
         )
         .then(
            function(text) {
               return JSON.parse(text);
            }
         )
         .then(
            function(json) {
               if (json.response != "ok")
                  throw json;
               return json.value;
            }
         )
         .catch(
            function(error)  {
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
            function(response) {
               return response.json()
            }
         )
         .then(
            function(json) {
               if (json.response != "ok")
                  throw json;
               return json.id ?
                  Id.fromKey(json.id) : json.key;
            }
         )
         .catch(
            function(error) {
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
   constructor(storage)
   {
      this._storage = storage;
      this._local =
         (storage.local == true);
   }
   
   setItem(key, value)
   {
      return Promise.resolve(this._storage.setItem(key, value));
   }
   
   getItem(key)
   {
      return Promise.resolve(this._storage.getItem(key));
   }
   
   removeItem(key)
   {
      return Promise.resolve(this._storage.removeItem(key));
   }
   
   clear()
   {
      return Promise.resolve(this._storage.clear());      
   }

   get url() {
      return this._storage.url;
   }

   set url(value) {
      this._storage.url = value;
   }
   
}

var storage = new Storage(remoteStorage);