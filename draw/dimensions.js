function Dimensions(input) {

   if (input == null)
      input = {}
      
   Object.assign(this, input);
   
   if (this.min == null)
      this.min = new Point(
         {
            x: Number.MAX_VALUE,
            y: Number.MAX_VALUE
         }
      );
      
   if (this.max == null)
      this.max = new Point(
         {
            x: -Number.MAX_VALUE,
            y: -Number.MAX_VALUE
         }
      );
      
   this.isPointInside = function(point) {
      return (this.min.x <= point.x    &&
              point.x    <= this.max.x &&
              this.min.y <= point.y    &&
              point.y    <= this.max.y);
   }
   
   this.contains = function(dimensions) {
      return (this.min.x <= dimensions.min.x &&
              this.min.y <= dimensions.min.y &&
              this.max.x >= dimensions.max.x &&
              this.max.y >= dimensions.max.y)
   }
   
   this.intersects = function(dimensions) {
   
      if (this.max.x >= dimensions.min.x &&
          this.min.x <= dimensions.max.x &&
          this.max.y >= dimensions.min.y &&
          this.min.y <= dimensions.max.y)
         return true;
      else
         return false;

   }
   
   this.getPoints = function() {
      var dimensions = this;
      var points = [
         // top left
         {
            x: dimensions.min.x,
            y: dimensions.max.y
         },
         // top right
         {
            x: dimensions.max.x,
            y: dimensions.max.y
         },
         // bottom right
         {
            x: dimensions.max.x,
            y: dimensions.min.y
         },
         // bottom left
         {
            x: dimensions.min.x,
            y: dimensions.min.y
         }
      ];
      return points;
   }
   
   Object.defineProperty(
      this,
      "points",
      {
         get: this.getPoints,
         enumerable: false
      }
   );
   
   this.getWidth = function() {
      return this.max.x - this.min.x;
   }
   
   Object.defineProperty(
      this,
      "width",
      {
         get: this.getWidth
      }
   );
   
   this.getHeight = function() {
      return this.max.y - this.min.y;
   }
   
   Object.defineProperty(
      this,
      "height",
      {
         get: this.getHeight,
         enumerable: false
      }
   );
   
   this.getTopLeft = function() {
      var topLeft = new Point(
         {
            x: this.min.x,
            y: this.max.y
         }
      );
      return topLeft;
   }
   
   Object.defineProperty(
      this,
      "topLeft",
      {
         get: this.getTopLeft,
         enumerable: false
      }
   );
   
   this.transform = function(matrix) {
   
      var min = glMatrix.vec2.fromValues(
         this.min.x, this.min.y
      );
      
      var max = glMatrix.vec2.fromValues(
         this.max.x, this.max.y
      );
      
      glMatrix.vec2.transformMat2d(
         min,
         min,
         matrix
      );
      
      glMatrix.vec2.transformMat2d(
         max,
         max,
         matrix
      );
      
      this.min.x = min[0];
      this.min.y = min[1];
      
      this.max.x = max[0];
      this.max.y = max[1];
      
   }
   
}

Dimensions.fromRectangle =
   function(input)
{
   var topLeft = input.topLeft;
   if (!topLeft)
      topLeft = new Point(
         {
            x: 0,
            y: 0
         }
      );
      
   var width = input.width;
   if (!width)
      width = 0;
      
   var height = input.height;
   if (!height)
      height = 0;
      
   var min = new Point(
      {
         x: topLeft.x,
         y: topLeft.y - height
      }
   );
   
   var max = new Point(
      {
         x: topLeft.x + width,
         y: topLeft.y
      }
   );
   
   var dimensions = new Dimensions(
      {
         min: min,
         max: max
      }
   );

   return dimensions;
}