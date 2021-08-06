class Collection extends Array {

   id;
   parent;

   constructor(input) {
      super();

      if (input == undefined)
         input = {}

      this.parent = input.parent;
            
      if (input.id == undefined)
         this.id = new Id({name: this.constructor.name});
      else
         this.id = new Id(input.id);

      if (input.keys != undefined) {        
         var collection = this;
         input.keys.forEach(
            key => collection.push(key)
         )
      }

   }

   push(item) {
      var pointer = this.getPointer(item);
      if (pointer != undefined)
         return super.push(pointer);
   }

   getPointer(item) {
      
      if (item == undefined)
         return undefined;

      var pointer;

      if (item instanceof Pointer)
         pointer = item;
      else if (item instanceof Id) {
         pointer = new Pointer({object: item});
      }
      else if (typeof item == "string") {
         pointer = new Pointer({key: item});
      }
      else if (typeof item == "object") {
         pointer = new Pointer(item);
      }

      if (pointer == undefined)
         throw new Error("Cant create Pointer from item.");

      return pointer;

   }

   keys() {
      var keys = [];
      this.forEach(
         pointer => {
            if (pointer)
               keys.push(pointer.key);
         }
      );
      return keys;
   }

   toJSON() {
      return {
         id: this.id,
         keys: this.keys()
      }
   }
}
