class Item extends Id {
   matrix;
   dimensions;
   children;
   dimensioned;
   label;
   value;
   index;
   parent;
   visible = true;
   selected = false;
      
   static _index = 0;
   
   constructor(input) {
      super(input ? input.id : null);

      if (input == undefined)
         input = {}

      if (input.matrix == undefined)
         this.matrix = new Matrix();
      else
         this.matrix =
            Matrix
            .fromJSON(input.matrix);

      if (input.children == undefined)
         this.children = new Children(this);
      else
         this.children = new Children(this, ...input.children);

      if (input.dimensions == undefined)
         this.dimensioned = false;
      else {
         this.dimensions =
            new Dimensions(input.dimensions);
         this.dimensioned = true;
      }

      if (input.parent)
         this.parent = input.parent;

      if (input.index == undefined)
         this.index = ++Item._index;
      else {
         this.index = input.index;
         if (this.index > Item._index)
            Item._index = this.index;
      }

      if (input.label == undefined)
         this.label = String(this.index);
      else
         this.label = input.label;

      this.value = input.value;
   }
   
   async hitTest(point, matrix) {
         
      var m = matrix.multiply(this.matrix);
      
      var dim =
         this.dimensions.matrixTransform(
            m
         );
         
      var hit =
           dim
           .isPointInside(point);
           
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
   
   pushMatrix(context)
   {
      var matrix =
         context.matrix.multiply(this.matrix);
      
      context.pushMatrix(matrix);
   }
   
   popMatrix(context) {
      return context.popMatrix();
   }

   async draw(context) {
      
      if (this.selected) {
         var rectangle = new Rectangle(this);
         await rectangle.draw(context);
      }

      return await this.children.draw(context);
   }

   async click(point) {
      alert("Parent: " + this.parent.label);
   }

   async remove() {
      
      var item = this;

      // Remove from parent
      var parentsChildren = this.parent.children;
      var index = parentsChildren.findIndex(value => value && (value.key == item.key));
      if (index != undefined) {
         parentsChildren[index] = undefined;
         this.parent.save();
      }

      // Remove our children
      this.children.remove();

      // Remove ourself
      super.remove();
   }

   toJSON() {
      return {
         id: super.toJSON(),
         index: this.index,
         label: this.label,
         value: this.value,
         dimensions: this.dimensions,
         matrix: this.matrix,
         children: this.children
      }
   }

}