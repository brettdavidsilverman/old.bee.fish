class Children extends Array {

   parent;

   constructor(parent, ...input) {
      super(...input);
      this.parent = parent;
      this.forEach(
         (item, index, array) => {
            if (item) {
               array[index] =
               new Pointer(item);
            }
         }
      );
   }
   
   async draw(context)
   {
      var children = await this.all();
     
      for ( var i = children.length - 1;
            i >= 0;
            --i )
      {
         var child = children[i];
         if (child.visible)
            await child.draw(context);
      }

   }
   
   toJSON() {
      var filter = this.filter(
         pointer => pointer != undefined
      )

      return filter.map(
         (child) => child.key
      );
   }
   
   async hitTest(point, matrix)
   {
      var children = await this.all();

      for ( var i = children.length - 1;
            i >= 0;
            --i )
      {
         var child = children[i];
         
         var hit = await child.hitTest(
            point, matrix
         );
         
         if (hit)
            return hit;
      }
      
      return null;
      
   }
   
   async findParent(child, matrix)
   {
 
      var children = await this.all();
      
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var test = children[i];
         
         var parent = await test.findParent(
            child, matrix
         );
         
         if (parent)
            return parent;
      }
      
      return null;
      
   }
  
   async findChildren(parent, matrix) {
   
      var foundChildren = new Map();
      
      var children = await this.all();
      
      children.forEach(
         child => {

            if ( child.isChild(
                    parent.dimensions,
                    matrix
                 ) )
            {
               foundChildren.set(child.key, child);
            }
        
         }
      );
      
      return foundChildren;
   }
   
   
   async all() {
      
      var children = this;

      var pointers = children.filter(
         pointer => pointer != undefined
      );

      var promises = pointers.map (
         pointer => pointer.fetch( { parent : children.parent } )
      );
      
      var all = await Promise.all(promises);

      return all;
      
   }
   

   remove() {
      this.forEach(
         child => {
            if (child) {
               child => child.remove()
            }
         }
      );
   }
  
}
