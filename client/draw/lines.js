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
   
   draw(context, matrix)
   {
      var all = this.all();
      
      all.then(
         lines => {
            lines.forEach(
               line => {
                  if (line)
                     line.draw(
                        context,
                        matrix.copy()
                     );
               }
            );
         }
      ).catch(
         (error) => {
            console.log(
               "Lines.draw " + error
            );
         }
      );

   }
   
   toJSON() {
      return this.map(
         (line) => {
            if (line)
               return line.key
         }
      );
   }
   
   async hitTest(point, matrix)
   {
      var all = this.all();
      var lines = await all;
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
   
   all() {
      
      var promises = this.map(
         pointer =>
            pointer.fetch()
      );
      
      var all = Promise.all(promises);

      return all;
      
   }
   
   
  
}
