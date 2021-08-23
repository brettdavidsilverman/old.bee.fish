class Pointer extends Id
{
   object = undefined;
   key = undefined;
   fetched = false;

   static map = new Map();

   constructor(input)
   {
      super(input ? input.id : null);

      if (input instanceof Pointer)
      {
         Object.assign(this, input);
      }
      else if (typeof input == "string")
         this.key = input;
      else if (input.object != undefined)
      {
         var object = input.object;
         
         if (object.key)
            this.key = object.key;
         else
            this.key =
               object.key =
               new Id(
                  {name: object.constructor.name}
               ).key;
               
         this.object = object;
         this.fetched = true;
         Pointer.map.set(this.key, this.object);
         console.log("Pointer:" + Pointer.map.size);
      }
      else if (input.key){
         this.key = input.key;
      }
   }

   async fetch(input)
   {
      if (this.fetched)
      {
         return this.object;
      }

      if (Pointer.map.has(this.key)) {
         var object = Pointer.map.get(this.key);
         this.object = object;
         this.fetched = true;
         return this.object;
      }

      var id = Id.fromKey(this.key);
      
      var object = await id.load(input)
      this.object = object;
      this.fetched = true;

      Pointer.map.set(this.key, this.object);
      console.log("Fetch:" + Pointer.map.size);
      
      return object;
   }

   toJSON()
   {
      return {
         id: super.toJSON(),
         key: this.key
      }
   }

   toString()
   {
      return JSON.stringify(
         this, null, "   "
      );
   }
   
   equals(pointer)
   {
      return pointer.key == this.key;
   }
   

   release() {
      delete this.object;
      this.fetched = false;
      Pointer.map.delete(this.key);
      console.log("~" + Pointer.map.size);
   }

}

