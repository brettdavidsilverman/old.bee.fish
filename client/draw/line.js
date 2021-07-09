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
         

      if (!this.dimensioned)
         this.dimensions = 
            this.getDimensions();
            

   }
  
   toJSON()
   {
      return {
         index: this.index,
         strokeStyle: this.strokeStyle,
         lineWidth: this.lineWidth,
         selected: this.selected,
         dimensions: this.dimensions,
         matrix: this.matrix,
         points: this.points,
         children: this.children
      }
   }
   
   async draw(context) {
      
      await super.draw(context);
      
      if (!this.clipContext(context)) {
         return;
      }
      
      var scale = context.matrix.scale();

      var lineWidth =
         this.lineWidth / scale;
         
      context.strokeStyle = this.strokeStyle;
      context.lineWidth = lineWidth;

      
      var start = this.points[0];
      context.font =
        String(20 / scale) +
        "px courier new";
      context.fillText(this.index, start.x, start.y);
      
      
      this.points.draw(context);
  
      context.popStack();
      
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
