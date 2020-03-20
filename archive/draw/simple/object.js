class ArrayWrapper {

   constructor(input) {
      this.id = input.id;
      this.array = input.array;
      this.isArrayWrapper = true;
      this.array.id = this.id;
   }
   
   static isArrayWrapper(object) {
      return (object instanceof
         ArrayWrapper) ||
         object.isArrayWrapper;
   }
}


// Convert an object to a string
// Use indent for formatting
// Use the children map to see 
// which child objects have been
// replaced with pointers.
// Children also stop circular references
Object.prototype.toString =
Array.prototype.toString =
   function(
      input = {
         indent: "   ",
         children: new Map(),
         short: true
      }
   )
{
   // Create the child map if not
   // already set
   var children = input.children;
   if (!children)
      children = new Map();
   
   var indent = input.indent;
   
   var short = input.short;
   if (short == undefined)
      short = true;

   // Convert this object to a string
   var string = JSON.stringify(
      this,
      filter,
      indent
   );

   return string;
   
   function filter(key, value) {
         
      if (value == null)
         return null;
         
      // stringify this object
      if (key == "")
         return value;
         
      // check for custom filters
      if ( typeof (this.hashFilter) ==
           "function" ) {
         value =
            this.hashFilter(
               key, value
            );
      }
      
      // pointers should be stringified
      // "as is"
      if (key == "->") {
      
         // if short form, just return
         // the ids name
         if (short)
            return value.id.name;
         
         
         return value; 
      }
      
      // the id values should br
      // stringified as is
      if (value instanceof Id) {
        
         // if short form, just return
         // the ids name
         if (short)
            return value.name;
            
         return value;
      }


      
      // all objects (except Id and arrays)
      // are replaced with pointers
      // and added to the children map
      if ( typeof value == "object" 
           && !ArrayWrapper
              .isArrayWrapper(this)
         )
      {
      
         // add object to child map
         if (!children.has(value)) {
            children.set(value, null);
         }
 
         // convert object to pointer
         value = {
            "->": {
               id: value.id,
               property: key
            }
         }
     
      }
  
      
      return value;
         
   }
 
}

// Filter out any properties
// starting with "_" from the
// json stringifier.
Object.prototype.hashFilter =
   function(
      key, value
   )
{
   return value;
}

// Gets or creates a new identifier
// Sets the id property
Object.defineProperty(
   Object.prototype,
   "id",
   {
      // Create the id for the first
      // time on this object.
      // Set the property such that
      // the id is fixed.
      get: function() {
   
         if (this instanceof Id)
            return undefined;
            
         // Create the id
         var id = new Id();
         
         id.name =
            this.constructor.name;
            
         // Set the property id
         this.id = id;
         
         return id;
   
      },
      // Replace the get/set with a
      // fixed value
      set: function(value) {
         Object.defineProperty(
            this,
            "id",
            {
               value: value,
               enumerable: true,
               writable: true
            }
         );
      },
      enumerable: true
   }
);

Object.defaultStorage = sessionStorage;


// Save an object to storage
Object.prototype.save =
   function (
      input = {
         storage: this._storage,
         children: new Map()
      }
   )
{
   
   if (!input)
      input = {}
      
   var storage = input.storage;
   if (!storage)
      storage = Object.defaultStorage;

   
   var children = input.children;
   if (!children)
      children = new Map();
 
   // create the key
   var key = this.id.key;
   
   var object;
   
   if ( Array.isArray(this) &&
        !ArrayWrapper.isArrayWrapper(this)) {
      object = new ArrayWrapper(
         {
            array: this,
            id: this.id
         }
      );

   }
   else
      object = this;
   
   // get the string value
   var string = 
      object.toString(
         {
            indent: "",
            children: children,
            short: false
         }
      );

   // save our string value to storage
   storage.setItem(
      key,
      string
   );

   // save all the children
   // marking each one as having been
   // saved
   for (var [child, value] of children)
   {
   
      if (value == undefined) {
      
         children.set(
            child,
            child.id.key
         );
         
         child.save(
            {
               storage,
               children
            }
         );
         
      }
      

   
   }

   return key;
 
}

// load an object from storage
Object.load = function(
   input = {
         key: undefined,
         storage: Object.defaultStorage,
         children: null
      }
   )
{
   var key = input.key;

   var storage = input.storage;
   if (!storage)
      storage = Object.defaultStorage;

   var children = input.children;
   if (!children)
      children = new Map();
   
   // see if this object has already
   // been loaded
   if (children.has(key)) {
      var object = children.get(key);
      return children.get(key);
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

   console.log(
      "Load " + object.id.name
   );
   
   // unwrap arrays
   if ( ArrayWrapper.isArrayWrapper(
           object
        )
      ) {
      var arrayWrapper =
         new ArrayWrapper(object);
      
      object = arrayWrapper.array;
   }
   
   
   // create a typed object,
   // that is a copy of input
   object = create(
      object,
      children,
      storage
   );
   
   // save the object to thein memory
   // map
   children.set(object.id.key, object);
   
   // set getters on the object to load
   // on demand
   var objects = new Map();
   setLoadOnDemand(object);

   // return the loaded object
   return object;
   
   // Replace properties pointers
   // with a lazy load, getter function
   // that loads on demand
   function setLoadOnDemand(object)
   {
      if (objects.has(object))
         return;
         
      objects.set(object, null);
      
      Object.keys(object).forEach(
      function(key) {
      
         var value = object[key];
         
         if ( value == null)
            return;
            
         if ( key == "id" && 
              value instanceof Id )
            return;
         
         if (Array.isArray(value))
         {
            var array = value;
            for ( var i = 0;
                  i < array.length;
                  i++ )
            {
               var item = array[i];
               if ( typeof item == 
                      "object" &&
                   "->" in item ) {
                  item = 
                     Object.load( {
                        key: item["->"].id.key,
                        children,
                        storage
                     } );
                  setLoadOnDemand(item);
                  array[i] = item;
               }
            }
            return;
         }
         else if ( typeof value == 
                      "object" &&
                   "->" in value )
         {
            var item = value["->"];
            var key = item.id.key;
            
            var getter = createGetter(
               key,
               children,
               storage,
               item.property
            );
            var setter = createSetter(
               item.property
            );
            
            Object.defineProperty(
               object,
               item.property,
               {
                  get: getter,
                  set: setter,
                  enumerable: true,
                  configurable: true
               }
            );
            
         }
         else if ( typeof value == "object")
            setLoadOnDemand(value);
         
         
      }
      
      );
      
      
   }
  
   
   function getObjectFromPointer(
      pointer,
      children,
      storage
   )
   {
      var key = pointer.id.key;
      var object = Object.load(
         {
            key,
            children,
            storage
         }
      );
      
      return object;
   }
   
   function createGetter(
      key,
      children,
      storage,
      property
   ) {
      return function() {
      
         var object = Object.load(
            {
               key,
               children,
               storage
            }
         );

         this[property] = object;
         
         return object;
      }
   }
   
   function createSetter(property) {
      return function(value) {
         Object.defineProperty(
            this,
            property,
            {
               value,
               writable: true
            }
         );
      }
   }

   function create(input, children, storage) {
      
      var object;
      var id = new Id(input.id);
      var builder =
            id.createBuilder();
      
      if (Array.isArray(input))
     {
         var array = input;
   
         // forward fetch array pointers
         for ( var i = 0;
               i < array.length;
               i++ ) 
         {
            var item = array[i];
            if ( typeof item == "object" &&
                  "->" in item )
            {
               var pointer = item["->"];
               item =
                  Object.load(
                     {
                        key:
                          pointer.id.key,
                        children,
                        storage
                     }
                  );
               array[i] = item;
            }
         }
   
         // create the object from its
         // builder, passing in an array
         // of arguments
         object = new builder(...array);
         
      }
      else {
      
         object = new builder(input, true);
      }
      
      object.id = id;

      return object;
   }
      

}