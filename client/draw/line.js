class Line extends Item {

   
   strokeStyle = "blue";
   lineWidth = 1.0;
   points;
   toolbox;
   
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
            
      if (input.toolbox == undefined)
         this.toolbox = new Pointer(
            {
               object: new ToolBox()
            }
         );
      else
         this.toolbox = new Pointer(
            {
               key: input.toolbox
            }
         );
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
         children: this.children,
         toolbox: this.toolbox
      }
   }
   
   static load(key)
   {
      return Id.load(Line, key);
   }
   
   async draw(context) {
      
      var matrix = this.getClippedMatrix(context);
      
      if (!matrix)
         return false;
         
      context.save();
      
      context.applyMatrix(matrix);
      
      var scale = matrix.scale();

      var lineWidth = 
         this.lineWidth / scale;
         
      context.strokeStyle = this.strokeStyle;
      context.lineWidth = lineWidth;
      
      if (this.selected) {
         var rectangle = new Rectangle(this);
         rectangle.draw(context);
      }

      
      var start = this.points[0];
      context.font =
        String(20 / scale) +
        "px courier new";
      context.fillText(this.index, start.x, start.y);
      
      
      this.points.draw(context);
  
      context.restore();
      
      return await super.draw(context);
      
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
