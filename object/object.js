Object.prototype.toString = objectToString;
const POINTERS = Symbol("pointers");

function objectToString()
{
   
   // Set a flag saying that get functions
   // should return poiinters, and not
   // trigger a fetch from memoru.
   this[POINTERS] = true;
   
  
  
   
   // Add each property to the json object
   var json = {}
   var object = this;
   Object.keys(this)
      .sort(compare)
      .forEach(addProperty);
   
   // Extract json string from json object
   var output = JSON.stringify(
      json,
      null,
      "   "
   );
   

   delete this[POINTERS];
   
   return output;
   
   // add each propery to the json object
   function addProperty(property) {
      var value = object[property];
      if ( (value instanceof Object) &&
           !(value instanceof Id) &&
           !(value instanceof Pointer) )
      {
         value = new Pointer(value);
      }
      json[property] = value;
   }
   
   // the id comes first
   function compare(property1, property2) {
      if (property1 == "=")
         return -1;
      return 0;
   }
}

// Helpful function for debugging.
// Returns the full object as a json
// formatted string.
// Circular references are replaced
// with null.
function ToString(input) {

   ShortHand.current = ShortHand.human;
   
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




