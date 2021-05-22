class Line extends App {

   points = [];
   strokeStyle = "blue";
   lineWidth = 1.0;
   
   constructor(input) {
      super(input);
      
      Object.assign(this, input);
      
      if (!input)
         input = {}
         
      if (!input.dimensions)
         this.calculateDimensions();
   }
  
   // pack the points into a
   // float64 array, remembering the
   // id.
   async toShorthand(shorthand) {
   
      var _points = await this.points;
      
      var points =
         new Float64Array(_points.length * 4);
         
      var index = 0;
      
      _points.forEach(
         (point) => {
            var timestamp =
               point["="];
            points[index++] =
               timestamp.time;
            points[index++] =
               timestamp.increment;
            points[index++] = point.x;
            points[index++] = point.y;
         }
      );
      
      var app = super.toShorthand(shorthand);
 
      console.log("Line.toShorthand.app");
     // console.log(app);
      
      var object = {
         "=": this["="]
                 .toShorthand(shorthand),
         points: points.toShorthand(shorthand),
         app: app
      }
      
      return object;
   }
  
   static fromStorage(object) {

      console.log("Line.fromStorage.start");
      
      var app = object.app;

      console.log(JSON.stringify(object));
      
      var array =
           Float64Array.fromStorage(
              object.points
           );
   
      var points =
         new Array(array.length / 4);
      console.log("Line.fromStorage.1")
      for ( var i = 0;
            i < array.length;
            i += 4 )
      { 
         points[i / 4] = getPoint(i);
      }
      
      app.points = points;
      
      app["="] = object["="];
      
      var line = new Line(app);
      console.log("Line.fromStorage.end")
      return line;
      
      function getPoint(index) {
   
         var timestamp = {
            time: array[index],
            increment: array[index + 1],
            name: "Point"
         }
         
         console.log(timestamp);
      
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
   async transform(matrix) {
      var points = await this.points;
      
      points.forEach(
         (point) => {
            matrix.transformPoint(
               point
            );
         }
      );
      
      var dimensions = await this.dimensions;
      dimensions.transform(matrix);
   }
   
   async draw(context) {
    
      alert("line draw");
      
      console.log("Line.draw.start");
      
      var _super = await super.draw(context);
      
      if (!_super)
         return false;
         
      await this.setStyle(this);
      
      var scale = context._scale;
      
      context.lineWidth = 
         this.lineWidth / scale;
     
      context.strokeStyle = this.strokeStyle;
      context.beginPath();
      
      var points = await this.points;
      
      var point = points[0];
      if (points.length == 1) {
         
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
            i < points.length;
            ++i )
      {
         var point = points[i];
      
         context.lineTo(
            point.x,
            point.y
         );

      }
      
      context.stroke();

      console.log("Line.draw.end");
      
      return true;
   }
   

   
   async calculateDimensions() {
   
      var points = await this.points;
      
      var min = this.points[0];
      var max = this.points[0];
    
      points.forEach(
         (point) => {
            min = Point.min(min, point);
            max = Point.max(max, point);
         }
      );

      var dimensions = await this.dimensions;
      
      if (!dimensions)
         this.dimensions = new Dimensions(
            {
               min,
               max
            }
         );
      else
      {
         dimensions.min = min;
         dimensions.max = max;
      }
   
   }

   async hitTest(point, event) {
      if (event != null &&
          !(event in this))
         return null;
         
      var dimensions = await this.dimensions;
      
      if ( dimensions
           .isPointInside(point) )
         return this;
   }
   
   async remove() {
      await super.remove();
   }


   

}
