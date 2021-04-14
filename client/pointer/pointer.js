class Pointer extends Id {
   _object;
  // #isArray;
   constructor(input) {
      super(
         "->" in input ?
            input["->"] :
            input["="]
      );
      
      if (!("->" in input))
         this._object = input;
   }
   
   fetch() {
   
      if (this._object) {
         return this._object;
      }

      var object = Memory.fetch(
         this.key,
         Memory.map
      );
      
      this._object = object;
      
      return object;
   }
   /*
   fetchArray(value, map) {
      
      var id = value["="];
      var map = Memory.map;
      var oldArray, newArray;
      if (map.has(id.key))
         oldArray = map.get(id.key);

      newArray = value["[]"];
      
      newArray.forEach(
         function(element, index) {
            if (Pointer.isPointer(element))
            {
               var p = new Pointer(element);
               newArray[index] =
                  p.fetch(map);
            }
         }
      );
      
      var array;
      if (oldArray) {
         Object.assign(oldArray, newArray);
         array = oldArray;
      }
      else {
         map.set(id.key, newArray);
         array = newArray;
      }
      
      return array;
   }
   */
   toShorthand(shorthand = Shorthand.HUMAN) {
      var name, key, timestamp;
      var output;
      
      if (shorthand & Shorthand.HUMAN) {
         output = {
            "->": this.name
         }
      }
      else {
         if (shorthand & Shorthand.COMPUTER)
            key = this.key;
      
         if (shorthand & Shorthand.FULL) {
            name = this.name;
            timestamp = this.timestamp;
         }
      
         var output = {
            "->": {
               name,
               key,
               timestamp
            }
         }
      }
     
      return output;
   }

   static isPointer(value) {
      var isPointer =
         (
            (value instanceof Pointer) || (
            (value instanceof Object) &&
            ("->" in value))
         );

      return isPointer;
      
   }
}

