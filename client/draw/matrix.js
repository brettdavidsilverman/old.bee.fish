var Matrix = DOMMatrix;

Matrix.prototype.toJSON =
   function() {
       return this.toString();
   }

Matrix.fromJSON =
   function(string) {
       return new Matrix(string);
   }

