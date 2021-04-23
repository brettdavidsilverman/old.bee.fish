var oldFunctionToString =
   Function.prototype.toString;
Function.prototype.toString =
   objectToString;

Function.prototype.toShorthand =
   function(shorthand) {
      var id = this["="];
      var string =
         oldFunctionToString.call(this);
      return {
         "=": id.toShorthand(shorthand),
         "=>": string
      }
   }

Function.fromStorage =
   function(object) {
      var string = object["=>"];
      var output = eval(
         "(" + string + ")"
      );
      Object.assign(output, object);
      delete output["=>"];
      return output;
   }