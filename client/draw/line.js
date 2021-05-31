class Line extends App {

   points = new Points([]);
   strokeStyle = "blue";
   lineWidth = 1.0;
   
   constructor(input) {
      super(input);
      
      if (!input)
         input = {}
        
      Object.assign(this, input);
      
      if (!input.dimensions)
         this.calculateDimensions();
      
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
