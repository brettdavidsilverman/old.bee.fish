class Pointer extends Id {
   #object;
   
   constructor(input) {
      super(getBaseId(input));
      
      if ("->" in input)
         this.isArray = false;
      else if ("[]" in input)
         this.isArray = true;
      else {
         this.#object = input;
         this.isArray = Array.isArray(
            input
         );
      }
      
      function getBaseId(input) {
         var id;
         
         if (input && input["->"]) {
            id = input["->"];
            if (id.name === undefined)
               throw new Error("pointer error");
         }
         else if (input && input["[]"]) {
            id = input["="];
         }
         else {
            id = input["="];
         }
         return id;
      }
      
   }
   
   fetch() {
   
      if (this.#object) {
         return this.#object;
      }

      var object = Memory.fetch(
         this.key
      );
      
      this.#object = object;
      
      return object;
   }
   
   fetchArray(value) {
      
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
                  p.fetch();
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
   
   toJSON() {
     
      var output;
     
      if (Shorthand.is(Shorthand.HUMAN))
         output = {
            "->": this.name
         }
      else if (Shorthand.is(
                  Shorthand.POINTERS)
              )
         output = {
            "->": {
               name: this.name,
               key: this.key
            }
         }
      else {
         output = {
            "->": {
               name: this.name,
               key: this.key
            }
         }
      }
      
      return output;
   }
 
   static isPointer(value) {
      var isPointer =
         (
            (value instanceof Object) &&
            (("->" in value) ||
             ("[]" in value))
         );

      return isPointer;
      
   }
}

