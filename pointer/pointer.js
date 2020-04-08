class Pointer extends Id {
   #object;
   
   constructor(input) {
      super(getBaseId(input));
      
      if (!("->" in input))
         this.#object = input;
         
      function getBaseId(input) {
         var id;
         
         if (input && input["->"])
            id = input["->"];
         else {
            id = Id.checkId(input);
         }
         return id;
      }
      
   }
   
   fetch(memory) {
   
      if (this.#object) {
         return this.#object;
      }

      var object = Memory.fetch(
         this.key,
         memory
      );
      
      this.#object = object;
      
      return object;
   }
   
   toJSON() {
      
      var output;
      
      switch (ShortHand.current)
      {
      case ShortHand.human:
         output = {
            "->": this.name
         }
         break;
      case ShortHand.state:
         output = {
            "->": {
               name: this.name,
               key: this.key
            }
         }
         break;
      case ShortHand.full:
      default:
         output = {
            "->": {
               name: this.name,
               ms: this.ms,
               inc: this.inc,
               key: this.key
            }
         }
         break;
      }

      return output;
   }
 

}

