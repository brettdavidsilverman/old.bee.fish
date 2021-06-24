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
   
   async draw(context, matrix)
   {
      var lines = await this.all();
      
      lines.forEach(
         line => {
            line.draw(
               context,
               matrix.copy()
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
         
         hit = line.hitTest(
            point, matrix.copy()
         );
         
         if (hit)
            return hit;
      }
      
      return null;
      
   }
   
   async contains(line, matrix)
   {
 
      var lines = await this.all();
      
      for ( var i = lines.length - 1;
            i >= 0;
            --i )
      {
         var test = lines[i];
         
         var contains = await test.contains(
            line, matrix.copy()
         );
         
         if (contains)
            return contains;
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
