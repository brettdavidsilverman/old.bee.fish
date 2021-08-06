class Pointer extends Id
{
   object = undefined;
   key = undefined;
   fetched = false;
   
   constructor(input)
   {
      super(input && input.id ? input.id : null);

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
      }
      else if (input.key)
         this.key = input.key;
   }

   async fetch(input)
   {
      if (this.fetched)
      {
         return this.object;
      }
      
      var id = Id.fromKey(this.key);
      
      var object = await id.load(input)
      this.object = object;
      this.fetched = true;

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
   

}

