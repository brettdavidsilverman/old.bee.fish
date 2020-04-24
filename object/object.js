Object.prototype.toString = objectToString;
Object.prototype.toJSON = objectToJSON;

function objectToString(shorthand = Shorthand.HUMAN)
{

   new Shorthand(shorthand);
   
   // Extract json string from json object
   var indent;
   if (Shorthand.is(Shorthand.POINTERS))
      indent = undefined;
   else
      indent = "   ";
      
   var output = JSON.stringify(
      this,
      null,
      indent
   );
   
   Shorthand.pop();
   
   return output;
  
 
}

function objectToJSON(key) {

   // If Id, or extends Id 
   // such as Pointer, return the
   // unchanged objecy
   if (this instanceof Id)
      return undefined;
      
   if (this instanceof Function)
      return undefined;
      
   var map = Shorthand.map;
   
   if (map.has(this)) {
      
      var pointer = new Pointer(this);
      
      return pointer.toJSON();
   }
   
   map.set(this);
   
   if (Shorthand.is(
          Shorthand.POINTERS)) {
       
      if (key != "" && key != "->" &&
          key != "[]" && key != "{}" &&
          key != "=")
      {
         var pointer = new Pointer(this);
         return pointer.toJSON();
      }
      
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




