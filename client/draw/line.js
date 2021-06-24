class Line extends Id {

   
   strokeStyle = "blue";
   lineWidth = 1.0;
   selected = false;
   matrix;
   dimensions;
   points;
   lines;
   
   constructor(input) {
      super(input);
      
      if (!input)
         input = {}
        
      if (input.points == undefined)
         this.points = new Points();
      else
         this.points =
            new Points(...input.points);
         
      if (input.matrix == undefined)
         this.matrix = new Matrix();
      else
         this.matrix =
            Matrix
            .fromJSON(input.matrix);

      if (input.lines != undefined)
      {
         this.lines = new Lines(...input.lines);
      }
      else
         this.lines = new Lines();

      if (input.dimensions == undefined)
         this.dimensions = 
            this.getDimensions();
      else
         this.dimensions =
            new Dimensions(input.dimensions);
            
      if (input.selected)
         this.selected = true;
   }
  
   toJSON()
   {
      return {
         strokeStyle: this.strokeStyle,
         lineWidth: this.lineWidth,
         selected: this.selected,
         dimensions: this.dimensions,
         matrix: this.matrix,
         points: this.points,
         lines: this.lines
      }
   }
   
   static load(key)
   {
      return Id.load(Line, key);
   }
   
   draw(context, matrix) {
   
    
      context.save();
      
      matrix.multiplySelf(this.matrix);
      
      context.applyMatrix(matrix);
      
      var scale = matrix.a;
      
      var lineWidth = 
         this.lineWidth / scale;
         
      context.strokeStyle = this.strokeStyle;
      context.lineWidth = lineWidth;
      
      context.clearRect(
         this.dimensions.topLeft.x - lineWidth,
         this.dimensions.topLeft.y - lineWidth,
         this.dimensions.width + lineWidth * 2,
         this.dimensions.height + lineWidth * 2
      );
      
      if (this.selected) {
         var rectangle = new Rectangle(this);
         rectangle.draw(context);
      }

      this.points.draw(context);
  
      context.restore();
      
      this.lines.draw(context, matrix.copy());
      
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

   hitTest(point, matrix) {
         
      matrix.multiplySelf(this.matrix);
      
      var inverse = matrix.inverse();
      
      var testPoint =
         point.matrixTransform(inverse);
      
      if ( this.dimensions
           .isPointInside(testPoint) )
         return this;
      
      return null;
   }
   
   async contains(line, matrix) {
         
      var m = matrix.copy();

      m.multiplySelf(this.matrix);
      
      var inverse = m.inverse();
      
      var lineDimensions =
         line.dimensions.matrixTransform(
            inverse
         );
      
      var contains =
        this.dimensions
        .contains(lineDimensions);
        
      if (contains) {
      
         var hit = await this.lines.contains(
            line, matrix.copy()
         );
         
         if (hit)
            return hit;
            
         return this;
      }
      
      return null;
   }
   

   

}
