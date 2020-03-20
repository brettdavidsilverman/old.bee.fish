class Id {
   // Creates a value that is
   // guaranteed yo be unique.
   // The value is milliseconds
   // and an increment for objects
   // created close in time 
   // to each other.
   // The increment is reset when
   // the milliseconds ticks over
   
   // Id also inludes a name
   // which is the name of the objects
   // constructor
   
   constructor(
      input = {
         milliseconds: Date.now(),
         increment: Id.increment,
         name: undefined
      }
   )
   {
      if (!input)
         input = {}
         
      this.milliseconds =
         input.milliseconds;
         
      if (!this.milliseconds)
         this.milliseconds = Date.now();
         
      if (this.milliseconds <=
          Id.milliseconds)
         Id.increment++;
      else {
         Id.increment = 0;
         Id.milliseconds = input.milliseconds;
      }
      
      this.increment = input.increment;
      if (!this.increment)
         this.increment =
            Id.increment;
      
      this.name = input.name;

   }
 
   
   static fromString(string) {
   
      var object = JSON.parse(
         string
      );
      
      return new Timestamp(
         object
      );
   }
   
   lessThenOrEqual(timestamp) {
      if (this.milliseconds < timestamp.milliseconds)
         return true;
      else if (this.milliseconds == timestamp.milliseconds)
         return this.increment <=
                timestamp.increment;
      else
         return false;
         
   }

   get key() {
      var key =
         "1" + // open branch
         this.milliseconds.encode() +
         this.increment.encode();
      return key;
   }
   
}





   /*

Object.load(
      input = {
         id: undefined,
         name: undefined,
         storage: defaultStorage
      }
   )
   {
      var id;
      
      if (input.name)
         id = new Builder(
            {
               name: input.name
            }
         );
      else {
         id = input.id;
      }

      var key = id.key;
      
      var storage = input.storage;
      if (!storage)
         storage = defaultStorage;
         
      var json =
         storage.getItem(key);

      var object;
      
      if (json == null)
         return object = {};
      else
         object = JSON.parse(
            json
         );
      
      var builder = id.getConstructor();
      
      var storable = new builder(
         object
      );
      
      storable.id = id;
      storable._storage = storage;
      
      // copy properties from retrieved
      // object to the constructored
      // object
      Object.keys(object).forEach(
         function(key) {
            if (!(key in storable))
               storable[key] = 
                  object[key];
         }
      );
      
      return storable;

   }
 */
Id.milliseconds = Date.now();
Id.increment = 0;

Storage.prototype.display = function()
{
   var storage = this;
   
   for ( var i = 0;
         i < storage.length;
         i++ )
   {
      var key = storage.key(i);
      var item = storage.getItem(key);
      console.log({key, item});
   }
}

