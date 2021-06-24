class Line extends Id {

   
   strokeStyle = "blue";
   lineWidth = 1.0;
   selected = false;
   matrix;
   dimensions;
   points;
   lines;
   count;
   
   static _count = 0;
   
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

      if (input.lines == undefined)
         this.lines = new Lines();
      else
         this.lines = new Lines(...input.lines);
         

      if (input.dimensions == undefined)
         this.dimensions = 
            this.getDimensions();
      else
         this.dimensions =
            new Dimensions(input.dimensions);
            
      if (input.selected)
         this.selected = true;
         
      if (input.count == undefined)
         this.count = ++Line._count;
      else {
         this.count = input.count;
         if (this.count > Line._count)
            Line._count = this.count;
      }
   }
  
   toJSON()
   {
      return {
         count: this.count,
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
   
   async draw(context, matrix, clipRegion) {
   
      
      var m = matrix.copy();
      
      m.multiplySelf(this.matrix);
      
      var inverse = m.inverse();
      
      var dim =
         this.dimensions.matrixTransform(
            m
         );

      if (!dim.intersects(clipRegion))
         return false;
         
      context.save();
      
      context.applyMatrix(m);
      
      var scale = m.a;
      
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
         "20 px courier new";
      context.fillText(this.count, start.x, start.y);
      
      
      this.points.draw(context);
  
      context.restore();
      
      await this.lines.draw(
         context, matrix.copy(), clipRegion
      );
      
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

   async hitTest(point, matrix) {
         
      var m = matrix.copy();
      
      m.multiplySelf(this.matrix);
      
      var inverse = m.inverse();
      
      var testPoint =
         point.matrixTransform(inverse);
      
      var hit = this.dimensions
           .isPointInside(testPoint);
           
      if (hit)
      {
         var child =
            await this.lines.hitTest(
               point, matrix.copy()
            );
        
         if (child)
            return child;
      
         return this;
      }
      
      return null;
   }
   
   async findParent(line, matrix) {
         
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
      
         var parent =
            await this.lines.findParent(
               line, matrix.copy()
            );
         
         if (parent)
            return parent;
            
         return this;
      }
      
      return null;
   }
   
   
   isChild(parentDimensions, matrix) {
   
      matrix.multiplySelf(this.matrix);

      var childDimensions =
         this.dimensions
         .matrixTransform(matrix);
         
      return parentDimensions
         .contains(
            childDimensions
         );
         
   }
   

   

}
