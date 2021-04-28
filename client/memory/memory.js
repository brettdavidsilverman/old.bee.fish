var Memory = {}
Memory.storage = new Storage(remoteStorage);
//Memory.storage = localStorage;
Object.prototype.save = saveObject;
Object.prototype.remove = removeObject;

// Save an object instance to storage
function saveObject(map = new Map(), promises = []) {
      
   var id = this["="];

   if (map.has(this))
      return Promise.resolve(id.key);
      
   map.set(this);

   // (Tell getters from fetch on demand
   // to return pointers)
   Memory.pointers = true;
   
 
   // Get the json representation
   // of this objects state
   var string = this.toString(
      Shorthand.COMPUTER
   );
   
   Memory.pointers = false;
   
   // Store the json string
   var promise =
      Memory.storage.setItem(
         id.key,
         string
      );
      
   promises.push(promise);
   
   var object = this;
   
   if (Array.isArray(this)) {
      this.forEach(
         (element) =>
            saveArrayElement(element)
      );
   }
   else {
      // Save the children.
      Object.keys(this).forEach(
         (property) =>
            saveChild(property)
      );
   }
   
   return Promise.all(promises);
   
   function saveArrayElement(value) {
      if ( value instanceof Object &&
         !(value instanceof Id) )
      {
         var promise =
            value.save(map, promises);
         promises.push(promise);
      }
   }
   
   function saveChild(property) {

      if (property == "=")
         return;
         
      var descriptor = Object
         .getOwnPropertyDescriptor(
            object,
            property
         );
         
      var value = descriptor.value;

      if ( value instanceof Object &&
           !(value instanceof Id) /* &&
           !(value instanceof Function) */)
      {
         var promise =
            value.save(map, promises);
         promises.push(promise);
         setPointer(object, property);
      }
   }
   
}

// Fetch an objects string from storage,
// and recreate the original object
Memory.fetch = function(
   key,
   map = new Map()
   )
{

   Memory.map = map;
   if (!Memory.map)
      Memory.map = new Map();
      
   var promise;
   
   // See if the object has been 
   // fetched before.
   if (Memory.map.has(key))
   {
      promise = Promise.resolve(
         Memory.map.get(key)
      );
      
      return promise;
   }
   
   // get the json object from storage
   promise =
   Memory.storage.getItem(key).then(
      function(string)
      {
   
         if (string === null)
         {
            throw new _KeyNotFoundError(key);
         }
   
         // Convert the json string to
         // an object.
         var json = JSON.parse(
            string
         );
         
         return json;
      }
   ).then(
      function(json)
      {
   
         // Get the id as this has the
         // type information
   
         var id = new Id(
            {
               key
            }
         );
   
         json["="] = id;

         // Create the class function
         // from the ids name.
         var type = id.type;
         
         // Construct the object using
         // either the copy constructor,
         // or the custom function
         var object;

         if (type.fromStorage
            instanceof Function)
         {
      
            // Use custom function
            object = 
              type.fromStorage(json);
         }
         else {
         
            // Use copy constructor
            object = new type(json);
            
         }
  
         // Save the typed object to memory
         Memory.map.set(id.key, object);
         if (!Array.isArray(object))
         {
            // Replace pointers with
            // fetch on demand getters
            Object.keys(object).forEach(
               function(property) {
                  setPointer(
                     object, property
                  );
               }
            );
        }
    
      
        return object;
     }
  )
  .catch(
     error => {
        if (error instanceof _KeyNotFoundError)
           return null;
        else
           throw error;
     }
  );
  
  return promise;
   

}
   
// Create Pointers to replace id's
// The pointer is only fetched from
// memory when needed.
// Functions are a special case and are
// evaluated immediatly
function setPointer(object, property) {
 
   // Get the value.
   var descriptor =
      Object.getOwnPropertyDescriptor(
         object,
         property
      );
   

   if (descriptor.value == undefined) {

      // Property has not been accessed,
      // its already in fetch on demand
      // mode
      return;
   }

   var value = descriptor.value;
   var newValue;
   
   if ( typeof value == "object" && 
             "=>" in value ) {
      // Create the function
      newValue = eval(
         "(" + value["=>"] + ")"
      );
            
      newValue["="] = value["="];
   }
   else if (!Pointer.isPointer(value))
      return;
   else
      // Create the typed pointer
      // object.
      newValue = new Pointer(value);

   // Set the read/write
   // functions on the object.
   Object.defineProperty(
      object,
      property,
      {
         //get: getter,
         // set: setter,
         value: newValue,
         enumerable: true
      }
   );
            

}

function removeObject() {
   return Memory.storage.removeItem(
      this["="].key
   );
}

class _KeyNotFoundError
{
   key;
   
   constructor(key) {
      this.key = key;
   }
}


