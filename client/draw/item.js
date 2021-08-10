class Item extends Id {
   dimensions;
   children;
   label;
   value;
   index;
   parent;
   inputs;
   outputs;
   visible = false;
   selected = false;
   matrix = new Matrix();

   static _index = 0;
   static map = new Map();
      
   constructor(input) {
      super(input ? input.id : null);

      if (input == undefined)
         input = {};

      this.parent = input.parent;

      var children;

      if (input.children == undefined)
         children = {}
      else
         children = input.children;

      children.parent = this;

      this.children =
         new Children(children);

      if (input.index == undefined)
         this.index = this.getNextIndex();
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

      if (input.dimensions)
         this.dimensions =
            new Dimensions(input.dimensions);
      
      var outputs  = input.outputs;
      if (outputs == undefined)
         outputs = {};
      outputs.parent = this;
      this.outputs = new Children(outputs);

   }
   
   getNextIndex() {
      return ++Item._index;
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
   
   async findParent(dimensions) {

      var contains =
         this.dimensions.contains(dimensions);
        
      if (contains) {
      
         var parent =
            await this.children.findParent(
               dimensions
            );
         
         if (parent)
            return parent;
            
         return this;
      }
      
      return null;
   }
   
   
   
   isChild(parent) {
      return parent.dimensions.contains(this.dimensions);
   }
   
   async draw(context) {
      
      if ( this.dimensions.intersects(context.dimensions) ) {
      
         if (!this.visible)
            this.show();
         
         if (this.selected) {
            var rectangle = new Rectangle(this);
            rectangle.fillStyle = "rgba(255, 255, 0, 0.5)";
            await rectangle.draw(context);
         }

         await this.children.draw(context);

         await this.outputs.draw(context);

         return true;
      }

      if (this.visible)
         this.hide();

      return false;
   }
   
   show() {
      if (this.index != undefined) {
         Item.map.set(this.key, this);         
      }
      this.visible = true;
   }

   async hide() {
      this.visible = false;
      Item.map.delete(this.key);
   }

   remove() {
      var self = this;

      // Remove from parent
      var siblings = this.parent.children;

      var index = siblings.findIndex((child) => child && (child.key == self.key));

      if (index >= 0) {
         siblings[index] = undefined;
      }

      // Recursively remove our children
      this.children.remove();

      // Remove ourself
      super.remove();

   }

   async click(point) {
      alert("Parent: " + this.parent);
   }
   
   toJSON() {
      return {
         id : super.toJSON(),
         index: this.index,
         label: this.label,
         value: this.value,
         dimensions: this.dimensions,
         matrix: this.matrix,
         children: this.children,
         outputs: this.outputs,
         output: this.output 
      }
   }

   toString()  {
      return this.label;
   }

}