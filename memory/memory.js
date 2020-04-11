var Memory = {}
Memory.storage = sessionStorage;
Object.prototype.save = save;
Object.prototype.remove = remove;

function save(map = new Map) {

   var id = this["="];
   
   if (map.has(this))
      return id.key;
      
   map.set(this);
   

   // Get the json representation
   // of this objects state
   var string = this.toString(Shorthand.POINTERS);
   
   // Store the json string
   Memory.storage.setItem(
      id.key,
      string
   );
  
   new Shorthand(Shorthand.POINTERS);
   var object = this;
   
   // Save the children.
   Object.keys(object).forEach(
      saveChildren
   );
   
   Shorthand.pop();
   
   return id.key;
   
   function saveChildren(property) {
      var value = object[property];
      if ( (value instanceof Object) &&
          !(value instanceof Id) &&
          !(value instanceof Pointer))
         value.save(map);
   }
}

Memory.fetch = function(
   key,
   memory = new Map()
   )
{
   // See if the object has been 
   // fetched before.
   if (memory.has(key))
      return memory.get(key);
      
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
   
   if ("[]" in json)
      json = json["[]"];
      
   // Get the id as this has the
   // type information
   var id = new Id(json["="]);
   json["="] = id;
   
   // Create the class function
   // from the ids name.
   var typeFunction = id.typeFunction;
   
   // Construct the object using
   // either the copy constructor,
   // or the custom function
   var object;

   if (typeFunction.fromJSON
       instanceof Function)
      // Use custom function
      object =
         typeFunction.fromJSON(json, memory);
   else
      // Use copy
      object =
         new typeFunction(json, memory);
      
   // Replace pointers with
   // fetch on demand getters
   Object.keys(object).forEach(
      setFetchOnDemand
   );
   
   
   // Save the typed object to memory
   memory.set(id.key, object);
      
   return object;

   // Create get (read) and set (write)
   // functions as fetch on demand.
   // The object is only fetched from
   // memory when needed.
   function setFetchOnDemand(property) {
  
      // Get the value.
      var value = object[property];
         
      // Check if the value is a pointer
      if (value instanceof Object &&
          "->" in value)
      {
            
         // Create the typed pointer
         // object.
         var pointer = new Pointer(
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

            if (Shorthand.type ===
                Shorthand.POINTERS) {
               // Dont fetch the object,
               // just return the pointer
               return pointer;
            }
       
            var fetched = pointer.fetch(
               memory
            );
           
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
                  value: setValue
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


