function Dimensions(input) {
   
   var dimensions = this;
   
   if (!input)
      input = {}
      
   if (input.min)
      this.min = input.min;
   else
      this.min = {
         x: Number.POSITIVE_INFINITY,
         y: Number.POSITIVE_INFINITY
      }

      
   if (input.max)
      this.max = input.max;
   else
      this.max = {
         x: Number.NEGATIVE_INFINITY,
         y: Number.NEGATIVE_INFINITY
      }
   
   
   
   this.round = function() {
      return new Dimensions(
         {
            min: Point.round(
               dimensions.min
            ),
            max: Point.round(
               dimensions.max
            )
         }
      );
   }
   
   this.draw = function(context) {
        
      context.fillRect(
         dimensions.left,
         dimensions.top, 
         dimensions.width,
         dimensions.height
      );
      
    
   }
   
   this.left = function() {
      return dimensions.min.x;
   }
   
   this.top = function() {
      return dimensions.min.y;
   }
   
   this.width = function() {

      return (dimensions.max.x -
              dimensions.min.x);
   }
   
   this.height = function() {
      return (dimensions.max.y -
              dimensions.min.y);
   }
   
   this.right = function() {
      return (dimensions.left +
              dimensions.width)
   }
   
   this.bottom = function() {
      return (dimensions.top +
              dimensions.height);
   }
   
   this.applyTo = function(element) {
      
      element.style.left = dimensions.left + "px";
      element.style.top = dimensions.top + "px";
      element.style.width = dimensions.width + "px";
      element.style.height = dimensions.height + "px";
   }
   
   this.toString = function() {
   
      var string =
         "{" +
            "min: " +
            Point.toString(dimensions.min) + 
            ", " +
            "max: " + 
            Point.toString(dimensions.max) +
         "}";

      return string;

   }
   
   this.transform = function(matrix) {
   
      var input = {
         min:
            Point.transform(
               matrix,
               min
            ),
         max:
            Point.transform(
               matrix,
               max
            )
      }
      
      return new Dimensions(input);
   }
   
   this.isPointInside = function(point) {
         
      var dim = dimensions;
      
      return (point.x >= dim.min.x
         && point.x <= dim.max.x
         && point.y >= dim.min.y
         && point.y <= dim.max.y);
      
   }

   
}