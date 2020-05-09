class Dimensions {

   constructor(input) {

      if (input == null)
         input = {}
      
      Object.assign(this, input);
   
      if (!this.min )
         this.min = new Point(
            {
               x: Number.MAX_VALUE,
               y: Number.MAX_VALUE
            }
         );
      
      if (!this.max)
         this.max = new Point(
            {
               x: -Number.MAX_VALUE,
               y: -Number.MAX_VALUE
            }
         );
   }
      
   isPointInside(point) {
      return (this.min.x <= point.x    &&
              point.x    <= this.max.x &&
              this.min.y <= point.y    &&
              point.y    <= this.max.y);
   }
   
   contains(dimensions) {
      return (this.min.x <= dimensions.min.x &&
              this.min.y <= dimensions.min.y &&
              this.max.x >= dimensions.max.x &&
              this.max.y >= dimensions.max.y)
   }
   
   intersects(dimensions) {
   
      if (this.max.x >= dimensions.min.x &&
          this.min.x <= dimensions.max.x &&
          this.max.y >= dimensions.min.y &&
          this.min.y <= dimensions.max.y)
         return true;
      else
         return false;

   }
   
   get points() {
      var dimensions = this;
      var points = Float64Array.from(
      [
         // top left
         0,0,
         dimensions.min.x,
         dimensions.max.y,
         
         // top right
         0,0,
         dimensions.max.x,
         dimensions.max.y,
         
         // bottom right
         0,0,
         dimensions.max.x,
         dimensions.min.y,
         
         // bottom left
         0,0,
         dimensions.min.x,
         dimensions.min.y
      ]);
      return points;
   }
   
   get width() {
      return this.max.x - this.min.x;
   }
   
   get height() {
      return this.max.y - this.min.y;
   }
   
   get topLeft() {
      var topLeft = new Point(
         {
            x: this.min.x,
            y: this.max.y
         }
      );
      return topLeft;
   }
   
   transform(matrix) {
      /*
      var min = glMatrix.vec2.fromValues(
         this.min.x, this.min.y
      );
      */
      var min = new DOMPoint(
         this.min.x,
         this.min.y
      );
      
      /*
      var max = glMatrix.vec2.fromValues(
         this.max.x, this.max.y
      );
      */
      var max = new DOMPoint(
         this.max.x,
         this.max.y
      );
      
      /*
      glMatrix.vec2.transformMat2d(
         min,
         min,
         matrix
      );
      */
      min = matrix.transformPoint(min);
      
      /*
      glMatrix.vec2.transformMat2d(
         max,
         max,
         matrix
      );
      */
      max = matrix.transformPoint(max);
      
      this.min.x = min.x;
      this.min.y = min.y;
      
      this.max.x = max.x;
      this.max.y = max.y;
      
   }
   

   static fromRectangle(input) {
   
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
}