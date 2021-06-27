class Item extends Id {
   matrix;
   dimensions;
   children;
   dimensioned;
   selected = false;
   count;
   
   static _count = 0;
   
   constructor(input) {
      super(input);

      if (input.matrix == undefined)
         this.matrix = new Matrix();
      else
         this.matrix =
            Matrix
            .fromJSON(input.matrix);

      if (input.children == undefined)
         this.children = new Children();
      else
         this.children = new Children(...input.children);

      if (input.dimensions == undefined)
         this.dimensioned = false;
      else {
         this.dimensions =
            new Dimensions(input.dimensions);
         this.dimensioned = true;
      }
      
      if (input.selected == true)
         this.selected = true;
         
      if (input.count == undefined)
         this.count = ++Item._count;
      else {
         this.count = input.count;
         if (this.count > Item._count)
            Item._count = this.count;
      }

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
            await this.children.hitTest(
               point, matrix
            );
        
         if (child)
            return child;
      
         return this;
      }
      
      return null;
   }
   
   async findParent(child, matrix) {
         
      var m = matrix.copy();

      m.multiplySelf(this.matrix);
      
      var inverse = m.inverse();
      
      var childDimensions =
         child.dimensions.matrixTransform(
            inverse
         );
      
      var contains =
        this.dimensions
        .contains(childDimensions);
        
      if (contains) {
      
         var parent =
            await this.children.findParent(
               child, matrix.copy()
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
   
   getClippedMatrix(context)
   {
      var matrix = context.matrix.copy();
      
      matrix.multiplySelf(this.matrix);
      
      var inverse = matrix.inverse();
      
      var dim =
         this.dimensions.matrixTransform(
            matrix
         );

      if (!dim.intersects(context.clipRegion))
         return null;
         
      return matrix;
   }
   
   draw(context) {
      return this.children.draw(context);
   }
   
}