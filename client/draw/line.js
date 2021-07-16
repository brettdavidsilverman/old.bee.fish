class Line extends Item {
   
   strokeStyle = "blue";
   lineWidth = 1.0;
   points;
   
   constructor(input) {
      super(input);
      
      if (!input)
         input = {}
        
      if (input.points == undefined)
         this.points = new Points();
      else
         this.points =
            new Points(...input.points);
         

      if (this.dimensions == undefined)
         this.dimensions = 
            this.getDimensions();
   }
  
   toJSON()
   {
      return {
         item: super.toJSON(),
         strokeStyle: this.strokeStyle,
         lineWidth: this.lineWidth,
         points: this.points
      }
   }
   
   async draw(context) {
      
      await super.draw(context);
      
      var scale = context.matrix.scale();

      var lineWidth =
         this.lineWidth / scale;
         
      context.strokeStyle = this.strokeStyle;
      context.lineWidth = lineWidth;

      
      var start = this.points[0];
      context.font =
        String(20 / scale) +
        "px courier new";
      
      var label;

      if (this.label != undefined)
         label = this.label;
      else
         label = this.index;

      context.fillText(label, start.x, start.y);
      
      
      this.points.draw(context);
  
      return true;
      
   }

   getDimensions() {
      
      var points = this.points;
      
      var min = points[0];
      var max = points[0];
    
      points.forEach(
         (point) => {
            min = Point.min(min, point);
            max = Point.max(max, point);
         }
      );

      return new Dimensions(
         {
            min,
            max
         }
      );

   }

   
   

}
