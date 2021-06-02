class Line extends Id {

   points;
   strokeStyle = "blue";
   lineWidth = 1.0;
   
   constructor(input) {
      super(input);
      
      if (!input)
         input = {}
        
      if (input.points == undefined)
         this.points = new Points(this, []);
      else
         this.points = new Points(this, input.points);
      /*
      if (!input.dimensions)
         this.calculateDimensions();
      */
   }
  
   toJSON()
   {
      return {
         time: this.time,
         increment: this.increment,
         strokeStyle: this.strokeStyle,
         lineWidth: this.lineWidth,
         points: this.points
      }
   }
   
   static async load(key)
   {
      return Id.load(Line, key);
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
   
   draw(context) {
    
      context.lineWidth = 
         this.lineWidth;
     
      context.strokeStyle = this.strokeStyle;
      context.beginPath();
      
      var points = this.points;
  
      var point = points[0];
      if (points.length == 1) {
         
         context.arc(
            point.x,
            point.y,
            this.lineWidth / 2,
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

      return true;
   }

   /*
   calculateDimensions() {
   
      var points = this.points;
      
      var min = points[0];
      var max = points[0];
    
      points.forEach(
         (point) => {
            min = Point.min(min, point);
            max = Point.max(max, point);
         }
      );

      var dimensions = this.dimensions;
      
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

   hitTest(point, event) {
      if (event != null &&
          !(event in this))
         return null;
         
      var dimensions = this.dimensions;
      
      if ( dimensions
           .isPointInside(point) )
         return this;
   }
   
    */
   

}
