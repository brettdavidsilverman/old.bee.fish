Matrix2d = {};

Matrix2d.identity = 
   new Matrix(
      [
         1, 0, 0,
         0, 1, 0,
         0, 0, 1
      ],
      3, 3
   );

Matrix2d.Translate = function(point) {
   return new Matrix(
      [
         1, 0, point.x,
         0, 1, point.y,
         0, 0, 1
      ],
      3, 3
   );
}

Matrix.prototype.translate =
function(point) {
   return this.multiply(
      new Matrix2d.Translate(point)
   );
}

Matrix2d.Scale = function(scale) {
   
   return new Matrix(
      [
         scale.x, 0, 0,
         0, scale.y, 0,
         0, 0, 1
      ],
      3, 3
   );
}

Matrix.prototype.scale =
   function(scale) {
      return this.multiply(
         new Matrix2d.Scale(
            scale
         )
      );
   }

Matrix.prototype.getScale =
   function() {
      return {
         x: this.array[0],
         y: this.array[4]
      }
   }

Matrix.prototype.transformPoint = function (point) {
   var pointMatrix = new Matrix(
      [
         point.x,
         point.y,
         1
      ],
      3, 1
   );
   
   var transformed = 
      this.multiply(pointMatrix);
      
   var newPoint =
      {
         x: transformed.array[0],
         y: transformed.array[1]
      }
   
   return newPoint;
}

Matrix.prototype.applyToContext =
function(context) {
      
   //  a, b, e
   //  c, d, f
   //  0, 0, 1
      
   //  0, 1, 2,
   //  3, 4, 5,
   //  6, 7, 8
      
   var m = this.array

   context.setTransform(
      m[0], // a
      m[1], // b
      m[3], // c
      m[4], // d
      m[2], // e
      m[5]  // f
   );

}
   
Matrix.prototype.applyToElement =
function(element) {

   var m = this.array;
      
   var transform =
      "matrix(" +
         m[0] + ", " + // a
         m[1] + ", " + // b
         m[3] + ", " + // c
         m[4] + ", " + // d
         m[6] + ", " + // e
         m[7] +        // f
      ")";
         

   element.style.transform = transform;
   
   element.style.transformOrigin = "top left";

}
   
 
