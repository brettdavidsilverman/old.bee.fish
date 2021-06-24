class Lines extends Array {

   constructor(...input) {
      super(...input);
      this.forEach(
         (element, index, array) => {
            array[index] =
               new Pointer(element);
         }
      );
   }
   
   async draw(context, matrix, clipRegion)
   {
      var lines = await this.all();
      
      await lines.forEach(
         async function(line) {
            await line.draw(
               context,
               matrix.copy(),
               clipRegion
            )
         }
      );

   }
   
   toJSON() {
      return this.map(
         (line) => line.key
      );
   }
   
   hitTest(point, matrix)
   {
      var lines = this.all();
 
      var hit;
      for ( var i = lines.length - 1;
            i >= 0;
            --i )
      {
         var line = lines[i];
         
         hit = line.hitTest(
            point, matrix.copy()
         );
         
         if (hit)
            return hit;
      }
      
      return null;
      
   }
   
   async hitTest(point, matrix)
   {
      var lines = await this.all();
      var hit;
      for ( var i = lines.length - 1;
            i >= 0;
            --i )
      {
         var line = lines[i];
         
         hit = await line.hitTest(
            point, matrix.copy()
         );
         
         if (hit)
            return hit;
      }
      
      return null;
      
   }
   
   async findParent(line, matrix)
   {
 
      var lines = await this.all();
      
      for ( var i = 0;
            i < lines.length;
            ++i )
      {
         var test = lines[i];
         
         var parent = await test.findParent(
            line, matrix
         );
         
         if (parent)
            return parent;
      }
      
      return null;
      
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
