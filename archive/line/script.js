function Line(input) {

   this.start = input.start;
   this.points = input.points ? input.points : [];
  
   var dimensions = null;
   
   this.getDimensions = function() {
   
      if (dimensions)
         return dimensions;
      
      var minimum = this.start;
   
      var maximum = this.start;

      this.points.forEach(
         function(point) {
     
            minimum = point.minimum(minimum);
      
            maximum = point.maximum(maximum);
         }
      );
      
      dimensions = new Dimensions(
         minimum,
         maximum
      );
      
      return dimensions;
   }

   this.clearDimensions = function() {
      dimensions = null;
   }
   
   
   this.draw = function(context) {
      context.beginPath();
      context.moveTo(this.start.x, this.start.y);
      this.points.forEach(
         function(point) {
      
            context.lineTo(point.x, point.y);
         }
      );
      context.stroke();
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
   
      var str = "Line(" + this.start
                + ",["
                + this.points.join(",")
                + "]"
                + ")";
      
      return str;
   }
   
   this.encode = function() {
      var length = (this.points.length + 1) * 2;
      var array = new Array(length);
      array[0] = this.start.x;
      array[1] = this.start.y;
      var i = 2;
      this.points.forEach(
         function(point) {
            array[i++] = point.x;
            array[i++] = point.y;
         }
      );
      
      return encode(array);
   }

}

Line.decode = function(input) {

   var string = input.string;
   
   var array = decode(string);
   var start = new Point(array[0], array[1]);
   
   var length = (array.length - 2) / 2;
   var points = undefined;
   
   if (length > 0) {
      points = new Array(length);
      for (var i = 2; i < length; i += 2) {
         var point = new Point(array[i], array[i + 1]);
         points[i / 2 - 1] = point;
      }
   }
   
   return new Line(
      {
         start: start,
         points: points
      }
   )
}