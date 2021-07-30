class Item extends Id {
   dimensions;
   children;
   label;
   value;
   index;
   parent;
   visible = true;
   selected = false;
   matrix = new Matrix();

   static _index = 0;
   
   constructor(input) {
      super(input);

      if (input == undefined)
         input = {}

      if (input.children == undefined)
         this.children = new Children(this);
      else {
         this.children = new Children(this, ...input.children);

      }

      if (input.dimensions)
         this.dimensions =
            new Dimensions(input.dimensions);

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

      if (input.matrix == undefined)
         this.matrix = new Matrix();
      else
         this.matrix = Matrix.fromJSON(input.matrix);

      this.value = input.value;

   }
   
   async hitTest(point) {
         
      var hit =
         this.dimensions.isPointInside(point);
           
      if (hit)
      {
         var child =
            await this.children.hitTest(
               point
            );
        
         if (child)
            return child;
      
         return this;
      }
      
      return null;
   }
   
   transform(matrix) {

   }

   async findParent(child) {
         
      var dim = this.dimensions.matrixTransform(this.matrix);
      var childDim = child.dimensions.matrixTransform(child.matrix);

      var contains =
         dim.contains(childDim);
        
      if (contains) {
      
         var parent =
            await this.children.findParent(
               child
            );
         
         if (parent)
            return parent;
            
         return this;
      }
      
      return null;
   }
   
   
   
   isChild(parent) {
      var parentDim = parent.dimensions.matrixTransform(parent.matrix);
      var childDim = this.dimensions.matrixTransform(this.matrix);

      return parentDim
         .contains(
            childDim
         );
         
   }
   
   async draw(context) {
      
      if (this.selected) {
         var rectangle = new Rectangle(this);
         await rectangle.draw(context);
      }

      context.pushMatrix(this.matrix);
      
      await this.children.draw(context);

      context.popMatrix();
   }
   

   async click(point) {
      alert("Parent: " + this.parent.label);
   }

   async remove() {
      var self = this;

      // Remove from parent
      var siblings = this.parent.children;
      var index = siblings.findIndex(child => child && (child.key == self.key));
      if (index >= 0) {
         siblings[index] = undefined;
         this.parent.save();
      }

      // Recursively remove our children
      this.children.remove();

      // Remove ourself
      super.remove();
   }

   toJSON() {
      return {
         ms: super.ms,
         inc: super.inc,
         index: this.index,
         label: this.label,
         value: this.value,
         dimensions: this.dimensions,
         matrix: this.matrix,
         children: this.children
      }
   }

}