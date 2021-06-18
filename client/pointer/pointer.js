class Pointer
{
   _object = undefined;
   _key = undefined;
   
   constructor(input)
   {
   
      
      if (input.object)
      {
         if (input.object.key)
            this._key = input.object.key;
         else
            this._key =
               input.object.key =
               new Id().key;
               
         this._object = input.object;
      }
      
      if (input.key)
         this._key = input.key;

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
      return {
         key: this.key
      }
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

