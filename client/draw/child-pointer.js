class ChildPointer extends Pointer {
   parent;

   constructor(input) {
      super(getPointer(input));

      this.parent = input.parent;

      function getPointer(input) {
         if (input && input.key)
            return input.key;
         else if (input && input.pointer)
            return input.pointer;
         else
            return input;
      }
   }

   toJSON() {
      return this.key
   }

   async fetch() {
      if (Item.map.has(this.key)) {
         return Promise.resolve(Item.map.get(this.key));
      }
      var item = await super.fetch();
      item.parent = this.parent;
      return item;
   }
}