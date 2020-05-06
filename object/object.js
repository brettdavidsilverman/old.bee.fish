Object.prototype.toString = objectToString;

// Function to convert an object to
// a string.
// Returns the full object as a json
// formatted string.
// References are replaced
// with pointers.
function objectToString(
   shorthand = Shorthand.HUMAN)
{
  
   var object =
      this.toShorthand(shorthand);
   
   var indent;
   
   if ( shorthand & Shorthand.HUMAN ||
        shorthand & Shorthand.FULL )
      indent = "   ";
      
   var output = JSON.stringify(
      object,
      null,
      indent
   );
   
   return output;
}

// Create an object copy of input
// that is only base level deep.
Object.prototype.toShorthand =
function(shorthand) {

   var input = this;
   var output = {};
   
   Object
      .keys(this)
      .sort(comparer)
      .forEach(
         addKey
      ); 

   return output;
   
   function comparer(a, b) {
      if (a == "=")
         return -1;
      if (a < b)
         return -1;
      if (a == b)
         return 0;
      if (a > b)
         return 1;
   }
   
   function addKey(key) {
         
      var value = input[key];
         
      if (value instanceof Object) {
         
         if ( key == "=") {
            // Id field
            var id = value;
            output["="] =
               id.toShorthand(shorthand);
         }
         else {
            // Replace object with a
            // pointer.
            var pointer;
            
            if (value instanceof Pointer)
               pointer = value;
            else
               pointer = new Pointer(value);
                  
            output[key] = 
               pointer.toShorthand(shorthand);
         }
            
      }
      else
         output[key] = value;
   }

}




