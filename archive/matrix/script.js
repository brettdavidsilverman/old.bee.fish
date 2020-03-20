function Matrix(iLength, jLength, array) {


   this.iLength = iLength;
   
   this.jLength = jLength;
      
   var a = this.array = array;

   this.name = iLength.toString() + "x" + jLength.toString();
   
   this.multiply = function(matrix) {
      return Matrix.multiply(this, matrix);
      
   }
   
   this.toString = function() {

         
      var a = this.array;
      var str = "[";
      
      //  loop through each row
      for (var i = 0; i < this.iLength; i++) {
         // get the row as a string
         var sub = a.slice(i * this.jLength, (i + 1) * jLength);
         
         // add the row
         str += sub.toString();
         
         // the last row is excluded
         // from adding a seperator and new line
         if (i < this.iLength - 1)
            str += ",\n";
      }
      str += "]";
      return str;
   }
      
}


Matrix.create = function(iLength, jLength) {
   var array = Array(iLength * jLength);
   var i, j, x;
   for (x = 0; x < iLength * jLength; x++) {
      i = Math.floor(x / jLength);
      j = x - i * jLength;
      
      array[x] = (i == j) ? 1 : 0;
   }
   
   var matrix = new Matrix(iLength, jLength, array);
 
   return matrix;
}

Matrix.multiply = function(a, b) {

   if (a.jLength != b.iLength)
      throw "Matrix dimensions are incompatible";
   
   var multiplierName = a.name + "." + b.name;
   
   // look up the cached multiplier
   var multiplier = Matrix[multiplierName];

   if (!multiplier) {
   
      // none in cache, so create one
      multiplier = createMultiplierFunction(a, b);
      
      // and store it on the cache
      Matrix[multiplierName] = multiplier;
   }
   
   //
   return multiplier(a, b);
   
   function createMultiplierFunction(a, b) {
      
      // extract the arrays
      // into a function string
      // that will later be evaluated
      // into a normal function
      var fstring = "a = a.array;\n";
      fstring += "b = b.array;\n";

      // get the length of the output
      // array
      var length = a.iLength * b.jLength;

      // initialize the out array
      // with values derived from
      // the input arrays
      fstring += "var out = [\n";

      var outij, i, j
      var ai, aj, aij, bi, bj, bij, k;
      
      var indent = new Indent(1);
      
      // for all values in our out
      // array
      for (outij = 0; outij < length; outij++) {
         // get the row of out
         i = Math.floor(outij / b.jLength);
         // get the column of out
         j = outij - i * b.jLength;
     
         // initialize sum of cell to 0
         // sum = 0
         fsumstring = "0";
         
         // create a sum of all the
         // values for the out cell
         for (k = 0; k < b.iLength; k++) {
            ai = i;
            aj = k;
            aij = ai * a.jLength + aj;
           
            bi = k;
            bj = j;
            bij = bi * b.jLength + bj;
         
           //sum += a.array[aij] * b.array[bij];
         
            fsumstring += " + (a[" + aij.toString() + "] * b[" + bij.toString() + "])";
;
         }
              
         // add this cells calculation to the rest
         fsumstring = indent.addTabs(fsumstring)
         
         fstring += fsumstring;
         
         if (outij < length - 1)
            fstring += ",\n";
         
      
//      out[outij] = sum;
      
      }
   
      
      fstring += "\n];\n";
      
      // convert the out array to an
      // out Matrix
      fstring += "out = new Matrix(" + a.iLength.toString() + ", " + b.jLength.toString() + ", out);\n"
  
      fstring += "return out;";
      fstring = new Indent(1, fstring);
      
      // Create the multiplier function
      var f = new Function("a", "b", fstring);
      
      // document.writeln(f);
      return f;
   }
   
}