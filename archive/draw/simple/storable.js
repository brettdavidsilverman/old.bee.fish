class Storable extends Object {
   constructor(
      input = { }
   ) {
   
      super(input);
      
      Object.assign(this, input);
      
      this.id.name =
         this.constructor.name;
         
   }
}