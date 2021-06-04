class Line extends Id {

   points;
   strokeStyle = "blue";
   lineWidth = 1.0;
   matrix;
   
   constructor(input) {
      super(input);
      
      if (!input)
         input = {}
        
      if (input.points == undefined)
         this.points = new Points(this, []);
      else
         this.points =
            new Points(this, input.points);
         
      if (input.matrix != undefined)
         this.matrix =
            Matrix
            .fromJSON(input.matrix);
      else
         this.matrix = new Matrix();
      
      /*
      if (!input.dimensions)
         this.calculateDimensions();
      */
   }
  
   toJSON()
   {
      return {
         strokeStyle: this.strokeStyle,
         lineWidth: this.lineWidth,
         points: this.points,
         matrix: this.matrix.toJSON()
      }
   }
   
   static load(key)
   {
      return Id.load(Line, key);
   }
   
   draw(context) {
   
    
      context.save();
      
      var matrix = Matrix.fromMatrix(
         context.matrix
      );
      
      matrix.multiplySelf(this.matrix);
      
      context.applyMatrix(matrix);
      
      var scale = matrix.a;
      
      context.lineWidth = 
         this.lineWidth / scale;
     
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

      context.restore();
      
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
