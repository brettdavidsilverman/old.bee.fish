var Memory = {}
//Memory.storage = remoteStorage;
Memory.storage = localStorage;
Object.prototype.save = saveObject;
Object.prototype.remove = remove;

// Save an object instance to storage
function saveObject(map = new Map()) {
      
   var id = this["="];

   if (map.has(this))
      return id.key;
      
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
   Memory.storage.setItem(
      id.key,
      string
   );
   
   var object = this;
   
   if (Array.isArray(this)) {
      this.forEach(
         saveElement
      );
   }
   else
      // Save the children.
      Object.keys(this).forEach(
         saveChildren
      );
   
   return id.key;
   
   function saveElement(value) {
      if ( value instanceof Object &&
         !(value instanceof Id) )
      {
         value.save(map);
      }
   }
   
   function saveChildren(property) {
   
      var descriptor = Object
         .getOwnPropertyDescriptor(
            object,
            property
         );
         
      var value = descriptor.value;
      if (  value instanceof Object &&
          !(value instanceof Id) )
      {
         value.save(map);
         setFetchOnDemand(object, property);
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
      
   // See if the object has been 
   // fetched before.
   if (Memory.map.has(key))
      return Memory.map.get(key);
      
   // get the json object from storage
   var string =
      Memory.storage.getItem(key);
   
   if (string === null) {
      return null;
   }
   
   // Convert the json string to
   // an object.
   var json = JSON.parse(
      string
   );
   
   // Get the id as this has the
   // type information
   
   var id = new Id(
      {
         name: json["="].name,
         key
      }
   );
   
   json["="] = id;
   
   // Create the class function
   // from the ids name.
   var Type = id.Type;
 
   // Construct the object using
   // either the copy constructor,
   // or the custom function
   var object;
   
   if (Type.fromStorage
       instanceof Function) {
      
      // Use custom function
      object = 
        Type.fromStorage(json);
   }
   else
      // Use copy constructor
      object = new Type(json);
  
   // Save the typed object to memory
   Memory.map.set(id.key, object);
   
   if (!Array.isArray(object))
      // Replace pointers with
      // fetch on demand getters
      Object.keys(object).forEach(
         function(property) {
            setFetchOnDemand(
               object, property
            );
         }
      );
    
      
   return object;
   

}
   
// Create get (read) and set (write)
// functions as fetch on demand.
// The property is only fetched from
// memory when needed.
function setFetchOnDemand(object, property) {
 
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
   
   var value = object[property];

   if (!Pointer.isPointer(value))
      return;
      
   // Create the typed pointer
   // object.
   var pointer = new Pointer(value);
         
   // Set the read/write
   // functions on the object.
   Object.defineProperty(
      object,
      property,
      {
         get: getter,
         set: setter,
         enumerable: true
      }
   );
            
   // Simply fetch the property
   // from memory and set it
   // back on the typed object.
   function getter() {
   
      if (Memory.pointers)
         return pointer;
         
      var fetched = pointer.fetch();

      this[property] = fetched;
            
      return fetched;
   }
            
   // Remove the getter/setter
   // and replace with the
   // fetched object.
   function setter(setValue) {
      Object.defineProperty(
         object,
         property,
         {
            value: setValue,
            writable: true
         }
      );
   }

}

function remove() {
   Memory.storage.removeItem(
      this["="].key
   );
}


