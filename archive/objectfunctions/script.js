Object.fromString = function(value) {

   return JSON.parse(value, parser);
   
   function parser(key, value) {
      if (typeof value == "string" && 
          value.startsWith("function"))
         return eval(
            "(" + value + ")"
         );
      else
         return value;
   }

}

Object.prototype.toString =
function() {

   return JSON.stringify(
      this,
      stringifier,
      "   "
   );
   
   function stringifier(key, value) {
      if (typeof value == "function")
         return new String(value);
      else
         return value;
   }
}

