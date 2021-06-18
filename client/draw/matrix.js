var Matrix = DOMMatrix;

Matrix.prototype.toJSON =
   function() {
       try {
          return this.toString();
       }
       catch (error)
       {
          console.log("Resetting matrix");
          return new Matrix().toString();
       }
   }

Matrix.fromJSON =
   function(string) {
       return new Matrix(string);
   }

Matrix.prototype.copy =
   function() {
      return Matrix.fromMatrix(this);
   }
   
