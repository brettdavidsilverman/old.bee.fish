class Pointer
{
   object = undefined;
   key = undefined;
   fetched = false;
   
   constructor(input)
   {
      if (input instanceof Pointer)
      {
         Object.assign(this, input);
      }
      else if (typeof input == "string")
         this.key = input;
      else if (input.object)
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
   
   
   fetch()
   {
      var pointer = this;
      
      if (this.fetched)
      {
         return Promise.resolve(this.object);
      }
      
      var promise = Id.load(this.key)
      .then(
         (object) => {
            pointer.object = object;
            pointer.fetched = true;
            return object;
         }
      );
      
      return promise;
   }

   toJSON()
   {
      return this.key;
   }

   toString(shorthand)
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

