class Children extends Array {

   constructor(...input) {
      super(...input);
      this.forEach(
         (element, index, array) => {
            array[index] =
               new Pointer(element);
         }
      );
   }
   
   async draw(context)
   {
      var children = await this.all();
     
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var child = children[i];
         
         await child.draw(context);
      }

   }
   
   toJSON() {
      return this.map(
         (child) => child.key
      );
   }
   
   async hitTest(point, matrix)
   {
      var children = await this.all();
      var hit;
      for ( var i = children.length - 1;
            i >= 0;
            --i )
      {
         var child = children[i];
         
         hit = await child.hitTest(
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
                    matrix.copy()
                 ) )
            {
               foundChildren.set(child.key, child);
            }
        
         }
      );
      
      return foundChildren;
   }
   
   
   async all() {
      
      var promises = this.map(
         pointer =>
            pointer.fetch()
      );
      
      var all = await Promise.all(promises);

      return all;
      
   }
   
   
  
}
