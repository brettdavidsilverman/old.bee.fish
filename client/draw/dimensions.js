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
      
   async isPointInside(point) {
      var min = await this.min;
      var max = await this.max;
      return (min.x  <= point.x    &&
              point.x <= max.x &&
              min.y   <= point.y    &&
              point.y <= max.y);
   }
   
   async contains(dimensions) {
      var min = await this.min;
      var max = await this.max;
      
      var dimensionsMin = await dimensions.min;
      var dimensionsMax = await dimensions.max;

      return (min.x <= dimensionsMin.x &&
              min.y <= dimensionsMin.y &&
              max.x >= dimensionsMax.x &&
              max.y >= dimensionsMax.y)
   }
   
   async intersects(dimensions) {
      var min = await this.min;
      var max = await this.max;
      
      var dimensionsMin = await dimensions.min;
      var dimensionsMax = await dimensions.max;
      
      if (max.x >= dimensionsMin.x &&
          min.x <= dimensionsMax.x &&
          max.y >= dimensionsMin.y &&
          min.y <= dimensionsMax.y)
      {
         console.log("intersects");
         return true;
      }
      else
      {
         console.log("no intersect");
         /*
         console.log("min" + min);
         console.log("max" + max);
         console.log("dimMin" + dimensionsMin);
         console.log("dimMax" + dimensionsMax);
         */
         return false;
      }

   }
   
   async points() {
      var dimensions = this;
      var min = await dimensions.min;
      var max = await dimensions.max;
      
      var points = Float64Array.from(
      [
         // top left
         0,0,
         min.x,
         max.y,
         
         // top right
         0,0,
         max.x,
         max.y,
         
         // bottom right
         0,0,
         max.x,
         min.y,
         
         // bottom left
         0,0,
         min.x,
         min.y
      ]);
      return points;
   }
   
   async width() {
      var min = await this.min;
      var max = await this.max;

      return max.x - min.x;
   }
   
   async height() {
      var min = await this.min;
      var max = await this.max;
      return max.y - min.y;
   }
   

   async left() {
      var min = await this.min;
      return min.x;
   }
   
   async top() {
      var max = await this.max;
      return max.y;
   }

   async topLeft() {

      var left = await this.left;
      var top = await this.top;
      
      var topLeft = new Point(
         {
            x: left,
            y: top
         }
      );
      
      return topLeft;
   }
   
   async transform(matrix) {
   
      var _min = await this.min;
      var _max = await this.max;
      
      var min = new DOMPoint(
         _min.x,
         _min.y
      );
      
      var max = new DOMPoint(
         _max.x,
         _max.y
      );
      
      min = matrix.transformPoint(min);
      max = matrix.transformPoint(max);
      
      _min.x = min.x;
      _min.y = min.y;
      
      _max.x = max.x;
      _max.y = max.y;
      
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