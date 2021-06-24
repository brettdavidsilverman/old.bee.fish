class Dimensions {

   constructor(input) {

      if (input == null)
         input = {}
      
      Object.assign(this, input);
   
      if (!input.min )
         this.min = new Point(
            {
               x: Number.MAX_VALUE,
               y: Number.MAX_VALUE
            }
         );
      
      if (!input.max)
         this.max = new Point(
            {
               x: -Number.MAX_VALUE,
               y: -Number.MAX_VALUE
            }
         );
   }
      
   isPointInside(point) {
   
      var min = this.min;
      var max = this.max;
      
      return (min.x  <= point.x    &&
              point.x <= max.x &&
              min.y   <= point.y    &&
              point.y <= max.y);
   }
   
   contains(dimensions) {
   
      var min = this.min;
      var max = this.max;
      
      return (min.x <= dimensions.min.x &&
              min.y <= dimensions.min.y &&
              max.x >= dimensions.max.x &&
              max.y >= dimensions.max.y)
   }
   
   intersects(dimensions) {
   
      var min = this.min;
      var max = this.max;
      
      if (max.x >= dimensions.min.x &&
          min.x <= dimensions.max.x &&
          max.y >= dimensions.min.y &&
          min.y <= dimensions.max.y)
      {
         return true;
      }
      else
      {
         return false;
      }

   }
   
   points() {
      var dimensions = this;
      var min = dimensions.min;
      var max = dimensions.max;
      
      var points
      [
         // top left
         new Point(
            {
               x: min.x,
               y: min.y
            }
         ),
         
         // top right
         new Point(
            {
               x: max.x,
               y: min.y
            }
         ),
         
         // bottom right
         new Point(
            {
               x: max.x,
               y: max.y
            }
         ),
         
         // bottom left
         new Point(
            {
               x: min.x,
               y: max.y
            }
         )
      ];
      
      return points;
   }
  
   get width() {
      return this.max.x - this.min.x;
   }
   
   get height() {
      return this.max.y - this.min.y;
   }
   

   get left() {
      return this.min.x;
   }
   
   get top() {
      return this.min.y;
   }

   get topLeft() {

      var topLeft = new Point(
         {
            x: this.left,
            y: this.top
         }
      );
      
      return topLeft;
   }
   
   transform(matrix) {
   
      var min = matrix.transformPoint(min);
      var max = matrix.transformPoint(max);
      
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