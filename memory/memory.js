var Memory = {}
Memory.storage = sessionStorage;
Object.prototype.save = saveObject;
Object.prototype.remove = remove;

function saveObject(map = new Map) {

   var id = this["="];
   
   Memory.map = map;
   
   if (map.has(this))
      return id.key;
      
   map.set(this);
   

   // Get the json representation
   // of this objects state
   var string = this.toString(
      Shorthand.POINTERS |
      Shorthand.ARRAY
   );

   // Store the json string
   Memory.storage.setItem(
      id.key,
      string
   );
  
   // Set shorthand to pointers so we
   // don't trigger any fetch on demand
   // properties.
   new Shorthand(Shorthand.POINTERS);
   
   var object = this;
   
   // Save the children.
   Object.keys(this).forEach(
      saveChildren
   );
   
   Shorthand.pop();
   
   return id.key;
   
   function saveChildren(property) {
      var value = object[property];
      if ( ((value instanceof Object) &&
          !(value instanceof Id) &&
          !(value instanceof Function)) ||
          Array.isArray(value))
         value.save(map);
   }
}

Memory.map = new Map();

Memory.fetch = function(
   key
   )
{
   var map = Memory.map;
   
   // See if the object has been 
   // fetched before.
   if (map.has(key))
      return map.get(key);
      
   // get the json object from storage
   var string = 
      Memory.storage.getItem(key);
   
   if (string === null)
      return null;
   
   // Convert the json string to
   // an object.
   var json = JSON.parse(
      string
   );
   
   
   // Get the id as this has the
   // type information
   var id = new Id(json["="]);
   json["="] = id;
  
   // Create the class function
   // from the ids name.
   var Type = id.Type;
 
   // Construct the object using
   // either the copy constructor,
   // or the custom function
   var object;
   
   if (Type.fromJSON
       instanceof Function) {
      
      // Use custom function
      object = 
        Type.fromJSON(json);
   }
   else
      // Use copy
      object = new Type(json);
  
   // Save the typed object to memory
   map.set(id.key, object);
   
   
   if (Array.isArray(object))
      fetchArrayItems(object);
   else
      // Replace pointers with
      // fetch on demand getters
      Object.keys(object).forEach(
         function(property) {
            setFetchOnDemand(object, property);
         }
      );
    
      
   return object;

   // Fetch pointer elements in
   // the areay
   function fetchArrayItems(array) {
      array.forEach(
         function(element, index) {
            if (Pointer.isPointer(element))
            {
               var pointer =
                  new Pointer(element);
               array[index] =
                  pointer.fetch();
            }
         }
      );
   }
   
   // Create get (read) and set (write)
   // functions as fetch on demand.
   // The property is only fetched from
   // memory when needed.
   function setFetchOnDemand(object, property) {
 
      // Get the value.
      var value = object[property];
      var pointer = null;

      // Check if the value is a pointer
      if (Pointer.isPointer(value)) {

         // Create the typed pointer
         // object.
         pointer = new Pointer(
            value
         );
         
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
           
            if (Shorthand.is(
                   Shorthand.POINTERS)) {
               // Dont fetch the object,
               // just return the pointer
               return pointer;
            }
       
            var newValue;
          
            var fetched = pointer.fetch();
            
            newValue = fetched;

            this[property] = newValue;
            
            return newValue;
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

   }
 
}

function remove() {
   Memory.storage.removeItem(
      this["="].key
   );
}


