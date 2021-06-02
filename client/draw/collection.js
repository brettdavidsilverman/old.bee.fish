class Collection extends Array {

   id;
   
   constructor(id, input) {
      super(...input);
      this.id = id;
   }
   
   save() {
      return storage.setItem(
         this.id.key,
         this
      );
   }
   
   static async load(type, key) {
      var value = await storage.getItem(key);
      var id = new Id({key});
      var array = new type(id, value);
      return array;
   }

  
}
