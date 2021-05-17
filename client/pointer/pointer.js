class Pointer
{
   _object = undefined;
   _pointerTo = undefined;
   
   constructor(input)
   {
   
      if (Pointer.isPointer(input))
      {
         if ("->" in input)
         {
            this._pointerTo =
               new Id(input["->"]);
         }
         else
            this._pointerTo = input._pointerTo;
            
         this._object = input._object;
      }
      else
      {
         this._pointerTo = input["="];
         this._object = input;
      }
   }
   
   get key()
   {
      return this._pointerTo.key;
   }
   
   async fetch()
   {
   
      if (this._object != undefined)
      {
         return Promise.resolve(this._object);
      }

      var pointer = this;
      
      var promise = Memory.fetch(
         this.key,
         Memory.map
      ).then(
         (object) => {
            pointer._object = object;
            return object;
         }
      );
      
      return promise;
   }

   toShorthand(shorthand = Shorthand.HUMAN)
   {
      var output =
         {
            "->":
               this._pointerTo.toShorthand(
                  shorthand
               )
         }
         
      return output;
   }

   toString(shorthand)
   {
      return JSON.stringify(
         this.toShorthand(shorthand)
      );
   }
   
   equals(pointer)
   {
      var bool =
         this._pointerTo    != undefined &&
         pointer            != undefined &&
         pointer._pointerTo != undefined &&
         this._pointerTo.equals(
            pointer._pointerTo
         );
         
      return bool;
   }
   
   static isPointer(value) {
      var isPointer =
         (
            (value instanceof Pointer) || (
               (typeof value == "object") &&
               ("->" in value)
            )
         );
      return isPointer;
      
   }
}

