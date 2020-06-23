class Line extends App {

   constructor(input) {
      super(input);
      
      if (!input)
         input = {}
         
      if (!input.points)
         this.points = [];
         
      if (input.strokeStyle == null)
         this.strokeStyle = "blue";
         
      if (input.lineWidth == null)
         this.lineWidth = 1.0;
         
      if (!input.dimensions)
         this.calculateDimensions();
   }
  
   // pack the points into a
   // float64 array, remembering the
   // id.
   toShorthand(shorthand) {
         
      var points =
         new Float64Array(this.points.length * 4);
         
      var index = 0;

      this.points.forEach(
         (point) => {
            var timestamp =
               point["="].timestamp;
            points[index++] = timestamp.ms;
            points[index++] = timestamp.inc;
            points[index++] = point.x;
            points[index++] = point.y;
         }
      );
        
      var object = {
         "=": this["="]
                 .toShorthand(shorthand),
         points: points.toShorthand(shorthand),
         App: super.toShorthand(shorthand)
      }
  
      return object;
   }
  
   static fromStorage(object) {
  
      var app = object.App;

      var array =
           Float64Array.fromStorage(
              object.points
           );
   
      var points =
         new Array(array.length / 4);
      
      for ( var i = 0;
            i < array.length;
            i += 4 )
      { 
         points[i / 4] = getPoint(i);
      }
      
      app.points = points;
      
      var line = new Line(app);
      
      return line;
      
      function getPoint(index) {
   
         var timestamp = {
            ms: array[index],
            inc: array[index + 1]
         }
      
         var id = new Id(
            {
               timestamp
            }
         );
      
         var point = new Point(
            {
               "=": id,
               x: array[index + 2],
               y: array[index + 3]
            }
         );
  
         return point;
      }
      
   }
   
   // transform all the points
   // and dimensions
   transform(matrix) {
      this.points.forEach(
         (point) => {
            matrix.transformPoint(
               point
            );
         }
      );
      this.dimensions.transform(matrix);
   }
   
   draw(context) {
    
      if (!super.draw(context))
         return false;
         
      this.setStyle(this);
      
      var scale = context._scale;
      
      context.lineWidth = 
         this.lineWidth / scale;
     
      context.strokeStyle = this.strokeStyle;
      context.beginPath();
      var point = this.points[0];
      if (this.points.length == 1) {
         
         context.arc(
            point.x,
            point.y,
            this.lineWidth / 2 / scale,
            0,
            Math.PI * 2
         );
         context.fillStyle = this.strokeStyle;
         
         context.fill();
      }
      
      context.moveTo(
         point.x,
         point.y
      );

      for ( var i = 1;
            i < this.points.length;
            ++i )
      {
         var point = this.points[i];
      
         context.lineTo(
            point.x,
            point.y
         );

      }
      
      context.stroke();

      
      return true;
   }
   

   
   calculateDimensions() {
   
      var min = this.points[0];
      var max = this.points[0];
    
      this.points.forEach(
         (point) => {
            min = Point.min(min, point);
            max = Point.max(max, point);
         }
      );

      if (!this.dimensions)
         this.dimensions = new Dimensions(
            {
               min,
               max
            }
         );
      else {
         this.dimensions.min = min;
         this.dimensions.max = max;
      }
   
   }

   hitTest(point, event) {
      if (event != null &&
          !(event in this))
         return null;
         
      if (this.dimensions
         .isPointInside(point))
         return this;
   }
   
   remove() {
      super.remove();
   }


   

}
