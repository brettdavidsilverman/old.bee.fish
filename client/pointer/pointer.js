class Pointer
{
   _object = undefined;
   _key = undefined;
   
   constructor(input)
   {
   
      if (input instanceof Pointer)
      {
         Object.assign(this, input);
      }
      else if (input.object)
      {
         var object = input.object;
         
         if (object.key)
            this._key = object.key;
         else
            this._key =
               object.key =
               new Id(
                  {name: object.constructor.name}
               ).key;
               
         this._object = object;
      }
      else if (input.key)
         this._key = input.key;
      else if (typeof input == "string")
         this._key = input;

   }
   
   get key()
   {
      return this._key;
   }
   
   async fetch()
   {
   
      if (this._object != undefined)
      {
         return Promise.resolve(this._object);
      }

      var pointer = this;
      
      var promise = Id.load(this.key)
      .then(
         (object) => {
            pointer._object = object;
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

