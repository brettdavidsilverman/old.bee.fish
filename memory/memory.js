var Memory = {}
Memory.storage = sessionStorage;
Object.prototype.save = saveObject;
Object.prototype.remove = remove;

function saveObject(map = new Map) {

   var id = this["="];
   
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
       instanceof Function) {
      
      // Use custom function
      object =
         typeFunction.fromJSON(json, memory);
   }
   else
      // Use copy
      object =
         new typeFunction(json, memory);
  
   // Pre fetch array items
   if (Array.isArray(object))
      object.forEach(
         function(element, index) {
            prefetchItem(object, element, index);
         }
      );
   else
      // Replace pointers with
      // fetch on demand getters
      Object.keys(object).forEach(
         function(property) {
            setFetchOnDemand(object, property, memory);
         }
      );
   
   
   // Save the typed object to memory
   memory.set(id.key, object);
      
   return object;

   // Prefetch item
   function prefetchItem(object, element, property) {

      if (Pointer.isPointer(element)) {
         var pointer = new Pointer(
            element
         );
         var value = pointer.fetch(
            memory
         );

         object[property] = value;
      }
   }
   
   // Create get (read) and set (write)
   // functions as fetch on demand.
   // The property is only fetched from
   // memory when needed.
   function setFetchOnDemand(object, property, memory) {
 
      // Get the value.
      var value = object[property];
      var pointer = null;

      // Check if the value is a pointer
      if (Pointer.isPointer(value)) {
         alert(property);
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
          
            var fetched = pointer.fetch(
                  memory
            );
            
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
                  value: setValue
               }
            );
         }
         
         function fetchArrayItems(array, memory) {
            array.forEach(
               function(element, index) {
                  if (Pointer.isPointer(element))
                  {
                     var pointer =
                        new Pointer(element);
                     array[index] =
                        pointer.fetch(memory);
                  }
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


