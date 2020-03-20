Matrix3x3 = {};
Matrix3x3.Identity = function() {
   return
      new Matrix(
         [
            1, 0, 0,
            0, 1, 0,
            0, 0, 1
         ],
         3, 3
      );
}

Matrix3x3.Translate = function(x, y) {
   return new Matrix(
      [
         1, 0, x,
         0, 1, y,
         0, 0, 1
      ],
      3, 3
   );
}

Matrix.prototype.translate3x3 =
function(x, y) {
   return this.multiply(
      new Matrix3x3.Translate(x, y)
   );
}

Matrix3x3.Scale = function(
   scaleX,
   scaleY) {
   
   return new Matrix(
      [
         scaleX, 0, 0,
         0, scaleY, 0,
         0, 0, 1
      ],
      3, 3
   );
}

Matrix.prototype.scale3x3 =
   function(scaleX, scaleY) {
      return this.multiply(
         new Matrix3x3.Scale(
            scaleX,
            scaleY
         )
      );
   }
   
Matrix.prototype.transformPoint2d = function (point) {
   var p = new Matrix(
      [
         point.x,
         point.y,
         1
      ],
      3, 1
   );
   
   var t = this.multiply(p);
   var newPoint =
      {
         x: t.array[0],
         y: t.array[1]
      }
   
   return newPoint;
}

Matrix.prototype.applyToContext2d =
   function(context) {
      
      //  a, c, e
      //  b, d, f
      //  0, 0, 1
      
      //  0, 1, 2,
      //  3, 4, 5,
      //  6, 7, 8
      
      var m = this.array

      context.setTransform(
         m[0], // a
         m[3], // b
         m[1], // c
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
            m[3] + ", " + // b
            m[1] + ", " + // c
            m[4] + ", " + // d
            m[2] + ", " + // e
            m[5] + // f
         ")";
         

      element.style.transform = transform;
   
      element.style.transformOrigin = "top left";

   }
