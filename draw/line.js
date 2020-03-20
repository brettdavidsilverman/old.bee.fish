function Line(input) {

   var line = this;
   
   App.call(this, input);
   
   if (input.points == null)
      this.points = [];

   if (input.strokeStyle == null)
      this.strokeStyle = "blue";
         
   if (input.lineWidth == null)
      this.lineWidth = 1.0;
   
   var App_draw = this.draw;
   
   this.draw = function(context) {
   
      if (App_draw.call(this, context) 
          == false)
         return false;
     
      var points = this.points;
      this.setStyle(context);
      context.beginPath();
      
      if (points.length == 1) {
         context.arc(
            points[0].x,
            points[0].y,
            this.lineWidth / 2 / scale,
            0,
            Math.PI * 2
         );
         context.fillStyle = this.strokeStyle;
         
         context.fill();
      }
      
      context.moveTo(
         points[0].x,
         points[0].y
      );

      for ( var i = 1;
            i < points.length;
            i++ )
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
   
   var App_style = this.setStyle;
   
   this.setStyle = function(context) {
      App_style.call(this, context);
      var scale = context._scale;
      context.strokeStyle = this.strokeStyle;
      context.lineWidth =
         this.lineWidth / scale;
   }
   
   this.calculateDimensions = function() {
      var min = this.points[0];
      var max = this.points[0];
      this.points.forEach(
         function(point) {
            min = Point.min(min, point);
            max = Point.max(max, point);
         }
      );
      
      if (this.dimensions == null)
         this.dimensions = new Dimensions(
            {
               min: min,
               max: max
            }
         );
      else {
         this.dimensions.min = min;
         this.dimensions.max = max;
      }
      
      if (this.parent != null)
         this.parent.calculateDimensions();
   }

   this.hitTest = function(point, event) {
      if (event != null &&
          !(event in this))
         return null;
         
      if (this.dimensions
         .isPointInside(point))
         return this;
   }
   
   
   if (input.dimensions == null)
      this.calculateDimensions();
      
}
