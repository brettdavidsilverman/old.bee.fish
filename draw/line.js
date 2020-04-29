class Line extends App {

   constructor(input) {
      super(input);
      
      if (!input)
         input = {}
   
      if (!input.points)
         this.points = new Float64Array();
      
      if (input.strokeStyle == null)
         this.strokeStyle = "blue";
         
      if (input.lineWidth == null)
         this.lineWidth = 1.0;
         
      if (!input.dimensions)
         this.calculateDimensions();
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
      var point = this.getPoint(0);
      if (this.points.length <= 2) {
         
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

      for ( var i = 4;
            i < this.points.length;
            i += 4 )
      {
         var point = this.getPoint(i);
      
         context.lineTo(
            point.x,
            point.y
         );
         
      }
      
      context.stroke();

      
      return true;
   }
   

   
   calculateDimensions() {
   
      var min = this.getPoint(0);
      var max = this.getPoint(0);
    
      for ( var index = 4;
            index < this.points.length;
            index += 4 )
      {
         var point = this.getPoint(index);
         
         min = Point.min(min, point);
         max = Point.max(max, point);
      }

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
   
   getPoint(index) {
   
      var timestamp = {
         ms: this.points[index],
         inc: this.points[index + 1]
      }
      
      var id = new Id(
         {
            timestamp
         }
      );
      
      var point = new Point(
         {
            "=": id,
            x: this.points[index + 2],
            y: this.points[index + 3]
         }
      );
  
      return point;
   }
   


      
}
