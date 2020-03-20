var Memory = {
   storage: sessionStorage,
   load: loadFromStorage
}


// Save an object to storage
Object.prototype.save = function(input) {

   if (!input)
      input = {
         memory:  new Map()
      }
   
   var storage = input.storage;
   if (!storage)
      storage = Memory.storage;
      
   var saved = input.memory;
   if (!saved)
      saved = new Map();
      
   // get the key
   var key = this.id.key;
   
   if (saved.has(this))
      return key;
   
   saved.set(this, null);
   
   var children = new Map();
   
   // get the string value
   Memory.saving = true;
      
   var string = 
      this.toString(
         {
            indent:    "",
            memory:    children,
            shortHand: "pointers"
         }
      );
 
   Memory.saving = false;
   
   // save our string value to storage
   // by key
   storage.setItem(
      key,
      string
   );
      
   // save all the children
   // marking each one as having been
   // saved
   children.forEach(
      function(value, object) {

         object.save(
            {
               memory:  saved,
               storage: storage
            }
         );
         
      }
   );
   
   return key;
 
}

// load an object from storage
function loadFromStorage(input) {

   var key = input.key;
   
   var id = new Id({key:key});
   var storage = input.storage;
   if (!storage)
      storage = Memory.storage;
      
   var memory = input.memory;
   if (!memory)
      memory = new Map();
      
   // see if this object has already
   // been loaded
   if (memory.has(key)) {
      var object = memory.get(key);
      return object;
   }
   
   // get the json string from storage
   var json = storage.getItem(key);
  
   if (json == null)
      // key not found, return null
      return null;

   // convert the json string to an
   // object
   var object = JSON.parse(
      json
   );

   // create a typed object,
   // that is a copy of input
   object = buildObject(
      {
         id: id,
         object: object
      }
   );

   // save the object to the main memory
   // map
   memory.set(key, object);
   
   // return the loaded object
   return object;
   
   function buildObject(input) {
      
      var object = input.object;
      var name = object.name;
      var builder;
      
      if (Array.isArray(object))
         forwardFetchArrayItems(object)
      else {
         var builder = object.getBuilder();
      
         if (builder == Float32Array) {
        
            delete object.name;
            
            // create the array
            var array =
               Object.values(object);
               
            object.name = name;
            
            // use the from method to copy
            // the array
            object = builder.from(array);
            
            // forward fetch array pointers
            forwardFetchArrayItems(object);

         }
         else {
            // Build all objects except arrays
            // from storage, as a copy
            // constructor
            object = new builder(object);
         }
      }
      
      object.id = input.id;
      object.name = name;
      
      // set getters on the object to load
      // on demand
      object.setLoadOnDemand(
         {
            memory: memory,
            storage: storage
         }
      );
      
      return object;
   }
   
   // Load all objects from the pointers
   // in the array.
   function forwardFetchArrayItems(array) {

      
      for ( var index = 0;
            index < array.length;
            index++ )
      {
         var item = array[index];
         
         if ( Pointer.isPointer(item) ) {
            var pointer = new Pointer(item);
            //alert(pointer.key);
            pointer.memory = memory;
            pointer.storage = storage;
            item = pointer.getObject();
            array[index] = item;
        
         }
      }
   }
   

}

Object.prototype.getBuilder = function() {
   if (!this.name)
      throw new Error("Invalid builder name");
   var f = new Function(
      "return " + this.name + ";"
   );
   return f();
}

Object.prototype.setLoadOnDemand = function(input)
{
   if (!input)
      input = {}
      
   var object = this;

   var memory = input.memory;
   if (!memory)
      memory = new Map();
      
   var storage = input.storage;
   if (!storage)
      storage = Memory.storage;
      
   Object.keys(object).forEach(
   
      function(property) {
      
         var value = object[property];
          
         if ( value == null )
            return;
            
         if ( Id.isId(value) )
            return;
         
         if (Pointer.isPointer(value)) {

            var pointer;
            if (value.constructor == Pointer)
               pointer = value;
            else
               pointer = new Pointer(value);
               
            pointer.memory = memory;
            pointer.storage = storage;
            loadPointerOnDemand(
               {
                  object: object,
                  property: property,
                  pointer: pointer
               }
            );
         }
    
      }
      
   );
   
   // Create a get/set function on
   // the property that loads on demand.
   function loadPointerOnDemand(input) {

      var object   = input.object;
      var pointer  = input.pointer;
      var property = input.property;

      var getter = createGetter(
         input
      );
            
      var setter = createSetter(
         input
      );
            
      Object.defineProperty(
         object,
         property,
         {
            get: getter,
            set: setter,
            configurable: true,
            enumerable: true
         }
      );
            
   }
   
   // Returns a function that loads
   // the object pointed to by the
   // pointer and replaces the property
   // with that object.
   function createGetter(
      input
   ) {
      return function() {
         var pointer = input.pointer;

         if (Memory.saving)
            return pointer;
         
         var property = input.property;
         var object = pointer.getObject();
         
         this[property] = object;
         
         return object;
      }
   }
   
   // create a setter function for
   // the property.
   function createSetter(input) {
      return function(value) {
         Object.defineProperty(
            this,
            input.property,
            {
               value,
               writable: true,
               enumerable: true
            }
         );
      }
   }
      
}

Object.prototype.remove = function(input) {

   if (!input)
      input = {}
      
   if (input.storage == null)
      input.storage = Memory.storage;
      
   
   input.storage.removeItem(this.id.key);
   
}


