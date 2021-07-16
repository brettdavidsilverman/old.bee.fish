class ChildPointer extends Pointer {
   parent;

   constructor(input) {
      super(input);

      this.parent = input.parent;
   }

   fetch(input) {
      var self = this;
      if (input == undefined)
         input = {};
      input.parent = this.parent;

      return super.fetch(input)
   }
}