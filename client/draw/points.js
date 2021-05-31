class Points extends Array {

   constructor(input) {
      super(...input);
      
      this["="];
   }
   
   toShorthand(shorthand) {
   
      var object =
      {
         "=": this["="].toShorthand(shorthand)
      }
      
      if ( shorthand & Shorthand.COMPUTER )
      {
         var array =
            new Float64Array(this.length * 4);
         
         var index = 0;
      
         this.forEach(
            (point) => {
               var timestamp =
                  point["="];
               array[index++] =
                  timestamp.time;
               array[index++] =
                  timestamp.increment;
               array[index++] = point.x;
               array[index++] = point.y;
            }
         );
         array = array.toShorthand(shorthand);
         array = array["[]"];
         object["[]"] = array;

      }
      else if ( shorthand & Shorthand.HUMAN ) 
      {
         var array = [];
         this.forEach(
            (item) => {
               if (item == undefined)
                  array.push(item);
               else {
                  array.push(
                     item.toShorthand(
                        shorthand
                     )
                  );
               }
            }
         );
         object["[]"] = array;
      }
      else
         return super.toShorthand(shorthand);

      return object;
   }
   
   static fromStorage(object) {
   
      var id = new Id("Float64Array");
      var data = object["[]"];
      var array = decodetypedArray(data, id);

  
      var points =
         new Array(array.length / 4);
         
      for ( var i = 0;
            i < array.length;
            i += 4 )
      { 
         points[i / 4] = getPoint(i);
      }

      return new Points(points);
      
      function getPoint(index) {
   
         var timestamp = {
            time: array[index],
            increment: array[index + 1],
            name: "Point"
         }
         
         var id = new Id(
            timestamp
         );
      
         var point = new Point(
            {
               "=": id,
               x: array[index + 2],
               y: array[index + 3]
            }
         );
  
         return point;
      }
      
   }
   
   saveChildren(map, promises)
   {
   }
      

  
}
