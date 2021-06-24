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
  /*
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
   */
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
   
   async findChildren(line, matrix) {

      var children = new Map();
      
      var m = matrix.copy();
      
      m.multiplySelf(this.matrix);
      
      var inverse = m.inverse();
      
      var parentDimensions =
         line.dimensions.matrixTransform(
            inverse
         );

      var lines = await this.lines.all();
      
      lines.forEach(
         child => {
            if ( child.isChild(
                    parentDimensions,
                    matrix.copy()
                 ) )
            {
               children[child.key] = child;
            }
        
         }
      );
      
      return children;
   }
   
   
   isChild(parentDimensions, matrix) {
   
      matrix.multiplySelf(this.matrix);
      var inverse = matrix.inverse();
      
      var childDimensions =
         this.dimensions
         .matrixTransform(inverse);
         
      return parentDimensions
         .contains(
            childDimensions
         );
   }
   

   

}
