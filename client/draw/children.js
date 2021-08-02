class Children extends Array {

   parent;

   constructor(parent, ...input) {
      super(...input);
      this.parent = parent;
      var self = this;
      this.forEach(
         (item, index, array) => {
            if (item && item instanceof Id) {
               item.parent = self.parent;
               array[index] =
                  new ChildPointer({parent: self.parent, object: item});
            }
            else if (typeof(item) == "string")
               array[index] = 
                  new ChildPointer({parent: self.parent, key: item});
            else
               array[index] = undefined;
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

         if (child instanceof Item) {
            await child.draw(context);
         }

      }

   }
   
   async transform(matrix)
   {
      var children = await this.all();
     
      for ( var i = 0;
            i <  children.length;
            ++i )
      {
         var child = children[i];
         child.transform(matrix);
      }

   }

   toJSON() {
      var filter = this.filter(
         pointer => pointer != undefined
      )

      return filter;
   }
   
   async hitTest(point)
   {
      var children = await this.all();

      var hit = null;

      for ( var i = children.length - 1;
            i >= 0;
            --i )
      {
         var child = children[i];

         if (child instanceof Item) {
            hit = await child.hitTest(
               point
            );
         }
         
         if (hit)
            return hit;
      }
      
      return null;
      
   }
   
   async findParent(child)
   {
 
      var children = await this.all();
      
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var test = children[i];
         
         var parent = await test.findParent(
            child
         );
         
         if (parent)
            return parent;
      }
      
      return null;
      
   }
  
   async findChildren(parent) {
   
      var foundChildren = new Map();
      
      var children = await this.all();
      
      children.forEach(
         child => {

            if ( child.isChild(
                    parent
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
         object => {
            if (object instanceof Pointer){
               return object.fetch();
            }
            else {
               return Promise.resolve(object);
            }
         }
      );
      
      var all = await Promise.all(promises);

      return all;
      
   }
   

   async remove() {
      var children = await this.all();
      children.forEach(
         child =>
            child.remove()
      );
   }
  
   async findParent(child)
   {
 
      var children = await this.all();
      
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var test = children[i];
         
         var parent = await test.findParent(
            child
         );
         
         if (parent)
            return parent;
      }
      
      return null;
      
   }
/*
   async resize(canvas)
   {
 
      var children = await this.all();
      
      for ( var i = 0;
            i < children.length;
            ++i )
      {
         var test = children[i];

         test.resize(canvas);
      }
      
      return null;
      
   }
*/
}
