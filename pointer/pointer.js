function Pointer(input) {

   if (!input)
      input = {}
 

   if (input.object) {
         
      // Creating a pointer to an object
      this.name = input.object.constructor.name;
      this.key = input.object.id.key;
     // this.object = input.object;
      
   }
   else {
      // Loading a copy of this
      // pointer from storage.
      this.key = input["->"];
   }
   
   this.getObject = function() {
   
      if (this.object) {
         return this.object;
      }

      var object = Memory.load(
         this
      );
      
      this.object = object;
      
      return object;
   }
   
   this.toObject = function(input) {
      
      if (!input)
         input = {}
      
      var output;
      
      if (input.shortHand == "short") {
         output = {
            "->": this.name
         }
      }
      else if (input.shortHand == "pointers") {
         output = {
            "->": this.key
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
   
}

Pointer.isPointer = function(object) {

   var isPointer = (
      (object) &&
      (typeof object == "object") &&
      (("->" in object) ||
       (object.constructor == Pointer))
   );

   return isPointer;
}

