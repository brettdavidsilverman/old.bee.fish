// Helper function to by pass toString
function JS(object) {
   var map = new Map();
   return JSON.stringify(object, replacer, 3);
   function replacer(property, value) {
   
      if (value == null)
         return null;
         
      if (typeof value == "object") {
         if (map.has(value))
            return null;
         
         map.set(value, null);
      }
      
      return value;
   }
}

// Helper function to shallow copy arrays
function copyArray(array) {
   return array.slice(0, array.length);
}

// Convert an object to a string
// Use indent for formatting
// Use the memory map to see 
// which child objects have been
// replaced with pointers.
// The memory map also stops
// circular references
Object.prototype.toString = toString;
Array.prototype.toString  = toString;
Float32Array.prototype.toString = toString;

Float32Array
   .prototype
   .toObject = arrayToObject;
Float32Array
   .prototype.bitsPerElement = 32;
   
Image.prototype.toString  = toString;

function arrayToObject(input) {
   return {
      "[]" : {
         type: typeof(this)
         //array: this.toBase64(),
        // fields: getFieldsFromArray(this)
      }
   }
}

function toString(input) {

   if (!input)
      input = {
         indent: "   ",
         memory: new Map(),
         shortHand: "short" // or "pointers"
      }
      
   // Create the memory map if not
   // already set
   var memory = input.memory;
   if (!memory)
      memory = new Map();
   
   if (memory.has(this))
     return;
     
   memory.set(this, null);
 
   var indent = input.indent;
   if (indent == undefined)
      indent = "   ";
   
   var shortHand = input.shortHand;
   if (!shortHand)
      shortHand = "short";

   var parameters = {
      shortHand: shortHand,
      memory:    memory
   }
   
   // Convert this object to a string
   var string = JSON.stringify(
      this,
      replacer,
      indent
   );

   return string;
   
   function replacer(property, value) {

      if (value != null &&
           typeof value == "object" &&
           !Id.isId(value))
         memory.set(value, null);

      // all objects
      // (except root, Id, pointers and
      //  arrays)
      // are replaced with pointers
      // and added to the memory map
      if ( (value       != null)      &&
           (value       != undefined) &&
           (typeof value == "object") &&
           //(!Pointer.isPointer(this)) &&
          // (!Pointer.isPointer(value))&&
           (!Id.isId(this))           &&
           (!Id.isId(value))          &&
           (property != "[]")         &&
           (property != "->")         &&
           (property.length > 0)
         )
      {

         // convert object to pointer
         value = new Pointer(
            {
               property: property,
               object: value
            }
         );
         
      }
                
      // check for custom filters
      if (value && value.toObject)
         value =
            value.toObject(parameters);
         
      
      return value;
         
   }
 
}

