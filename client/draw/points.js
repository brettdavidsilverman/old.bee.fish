class Points extends Collection {

   constructor(id, input) {
      super(id, input);
      for ( var i = 0; i < this.length; ++i )
      {
         if (this[i] != undefined)
         {
            this[i] = new Point(this[i]);
         }
      }
   }
   
   static load(key) {
      return Collection.load(Points, key);
   }

  
}
