function Line(input) {

   var line = this; 
   var dimensions = null;
   
   if (input && input.start)
      this.start = input.start;
   else
      this.start = {
         x: 0,
         y: 0
      }

   if (input && input.points)
      this.points = input.points;
   else
      this.points = [];
   
   
   this.ondraw = function(context) {}
   
   this.getDimensions = function() {
   
      if (!dimensions)
         this.calculateDimensions();
         
      return dimensions;
   }
   
   this.calculateDimensions = function() {

      var min = line.start;
   
      var max = line.start;

      this.points.forEach(
         function(point) {
     
            min = Point.min(
               min,
               point
            );
     
            max = Point.max(
               max,
               point
            );
            
         }
         
      );
      
      dimensions = new
         Dimensions(
            {
               min: min,
               max: max
            }
         );
      
      return dimensions;
   }

   this.resetDimensions = function() {
      dimensions = null;
   }
   
   
   this.draw = function(context) {

      context.moveTo(this.start.x, this.start.y);
      
      this.points.forEach(
         function(point) {
            context.lineTo(point.x, point.y);
         }
      );

      this.ondraw(context);
   }
   
   this.averageDistance = function(toPoint) {
   
      var sum = this.start.distance(toPoint);
      var count = 1;
      
      this.points.forEach(
         function(point) {
            sum += point.distance(toPoint);
            count++;
         }
      );
      
      return sum / count;
   }
   
   this.toString = function() {
   
      return JSON.stringify(this);
   }
   
   return this;
   
}