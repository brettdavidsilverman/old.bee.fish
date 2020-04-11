Object.prototype.toString = objectToString;
Object.prototype.toJSON = objectToJSON;

function objectToString(type = Shorthand.HUMAN)
{

   new Shorthand(type);
   
   // Extract json string from json object
   var output = JSON.stringify(
      this,
      null,
      "   "
   );
   
   Shorthand.pop();
   
   return output;
  
 
}

function objectToJSON(key) {

   if (this instanceof Id)
      return this;
    
   if (this instanceof Pointer)
      return this;
   
   var map = Shorthand.map;
   
   if (map.has(this)) {
      
      var pointer = new Pointer(this);
      
      return pointer.toJSON();
   }
   
   map.set(this);

   if (key != "" && key != "->" &&
       key != "*" && key != "{}" &&
       key != "[]" &&
        (Shorthand.type ===
         Shorthand.POINTERS)) {
      var pointer = new Pointer(this);
      return pointer.toJSON();
   }
   
   return this;
 
}

// Helpful function for debugging.
// Returns the full object as a json
// formatted string.
// Circular references are replaced
// with null.
function ToString(input) {

   Shorthand.current = Shorthand.human;
   
   var memory = new Map();
   
   var json = getObject(input);
   
   return JSON.stringify(
      json,
      null,
      "   "
   );
   
   function getObject(input) {
      var output = {};
      Object.keys(input).forEach(
         addKey
      );
      
      return output;
      
      function addKey(key) {
         var value = input[key];
         if (value instanceof Object) {
            if ((key != "->") &&
                (key != "=")) {
   
               if (memory.has(value)) {
                  output[key] =
                     new Pointer(value);
               }
               else {
                  memory.set(value);
                  output[key] = getObject(value);
               }
        
            }
         }
         else
            output[key] = value;
      }
     
   }
   


}




