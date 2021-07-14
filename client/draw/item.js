class Item extends Id {
   dimensions;
   children;
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

      if (input.children == undefined)
         this.children = new Children(this);
      else
         this.children = new Children(this, ...input.children);

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

      this.value = input.value;
   }
   
   async hitTest(point) {
         
      var hit =
           this.dimensions
           .isPointInside(point);
           
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
   
   async findParent(child) {
         
      
      var contains =
         this.dimensions
         .contains(child.dimensions);
        
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
   
      return parent.dimensions
         .contains(
            this.dimensions
         );
         
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
         children: this.children
      }
   }

}