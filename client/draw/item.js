class Item extends Id {
   matrix;
   dimensions;
   children;
   dimensioned;
   selected = false;
   index;
   
   static _index = 0;
   
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
         
      if (input.index == undefined)
         this.index = ++Item._index;
      else {
         this.index = input.index;
         if (this.index > Item._index)
            Item._index = this.index;
      }

   }
   
   async hitTest(point, matrix) {
         
      var m = matrix.multiply(this.matrix);
      
      var dim =
         this.dimensions.matrixTransform(
            matrix
         );
         
      var hit =
           dim.isPointInside(point);
           
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
         
      var m = matrix.multiply(this.matrix);
      
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
               child, matrix
            );
         
         if (parent)
            return parent;
            
         return this;
      }
      
      return null;
   }
   
   
   
   isChild(parentDimensions, matrix) {
   
      var m =
         matrix.multiply(this.matrix);

      var childDimensions =
         this.dimensions
         .matrixTransform(m);
         
      return parentDimensions
         .contains(
            childDimensions
         );
         
   }
   
   getClippedMatrix(context)
   {
      var matrix =
         context.matrix.multiply(this.matrix);
      
      var dim =
         this.dimensions.matrixTransform(
            matrix
         );

      if (dim.intersects(context.clipRegion))
         return matrix;
         
      return null;
   }
   
   draw(context) {
      
      return this.children.draw(context);
   }
   
}