var Matrix = DOMMatrix;
Matrix.prototype.save = saveObject;
Matrix.fromStorage = matrixFromStorage
Matrix.prototype.toString = objectToString;
Matrix.prototype.toShorthand =
function(shorthand) {
   var object;
   
   var m = this.toFloat64Array();
      
      
   object = {
      "=": this["="]
              .toShorthand(shorthand),
      "M": m.toShorthand(shorthand)
   }
   
   return object;
}

function matrixFromStorage(json) {
   
   var array = Float64Array.fromStorage(
      json["M"]
   );
   
   var matrix =
      Matrix.fromFloat64Array(array);
   matrix["="] = new Id(json["="]);
   
   return matrix;
}