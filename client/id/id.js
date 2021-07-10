class Id {
 
   ms = undefined;
   inc  = undefined;
   key  = undefined;
   name = undefined;
   
   // Creates a value that is
   // guaranteed to be unique
   // from this device.
   // The value is milliseconds (time)
   // and an increment (increment)
   // for objects created close
   // in time to each other.
   // The increment is reset when
   // the milliseconds ticks over
   constructor(input) {
   
      Object.assign(this, input);

      if ( this.key &&
             ( this.ms == undefined ||
               this.inc  == undefined ||
               this.name == undefined ) )
         this._decodeKey();
      else if ( this.ms == undefined ||
                this.inc == undefined )
         this._createTimestamp();
      
      if ( this.name == undefined )
      {
         this.name = this.constructor.name;
      }
      
      if ( this.key == undefined )
      {
         Object.defineProperty(
            this, 
            "key",
            {
               get() {
                  var key = this._encodeKey();
                  return key;
               },
               enumerable: true,
               configurable: true
            }
         );
      }
      

   }
   
   static fromKey(key) {
      return new Id({key});
   }
   
   _createTimestamp() {
      // create a new timestamp
      var milliseconds = Date.now();
      
      if ( milliseconds === Id.milliseconds )
         ++Id.increment;
      else {
         Id.increment = 0;
         Id.milliseconds = milliseconds;
      }
   
      this.ms = milliseconds;
      this.inc = Id.increment;

   }

   _encodeKey()
   {

      if ( this.ms == undefined ||
           this.inc == undefined )
         this._createTimestamp();
         
      var stream = new PowerEncoding();
      
      // Start bit
      stream.write("1");
      
      // encode name
      this.name.encode(stream);
      
      // encode time
      this.ms.encode(stream);
      
      // encode incrementrement
      this.inc.encode(stream);
      
      // end bit
      stream.write("0");
      
      var data = stream.toString();
      
      var key = btoa(data);
      
      Object.defineProperty(
         this, 
         "key",
         {
            value: key,
            enumerable: true,
            configurable: true
         }
      );
      
      return key;
         
   }
      
   _decodeKey() {
   
      if (this.key == undefined)
      {
         this._encodeKey();
         return;
      }
      
      // extract the name and timestamp
      // from the key
      var data = atob(this.key);
      
      var stream = new PowerEncoding(
         data
      );
         
      CHECK(
         "Id._decodeKey start bit",
         stream.read() == "1"
      );
      
      // read the name
      this.name = String.decode(stream);

      // read the time
      this.ms = Number.decode(stream);
      
      // read the incrementrement
      this.inc = Number.decode(stream);
      
      CHECK(
         "Id._decodeKey end bit",
         stream.read() == "0"
      );
      
      CHECK(
         "Id._decodeKey count",
         stream.count == 0
      );


   }
   
   toString() {
      return JSON.stringify(
         this,
         null,
         "   "
      );
   }
   
   static getType(name) {
   
      if (Id.types[name])
         return Id.types[name];
         
         
      Id.types[name] =
         createType(name);
      
      return Id.types[name];
      
      function createType(name) {
         var f = new Function(
            "return " + name + ";"
         );
         return f();
      }
      
   }
   
   toJSON() {
      return {
         name: this.name,
         ms: this.ms,
         inc: this.inc
      }
   }
   
   save() {
      console.log("Saving " + this);

      var id = this;
      var value = this;

      return storage.setItem(
         id,
         value
      );
   }
   
   remove () {
      var id = this;
      return storage.removeItem(
         id
      );
   }

   async load() {
      var value = await storage.getItem(this);
  
      if (value == undefined)
         return null;
         
      value.key = this.key;
      
      var type = Id.getType(this.name);
      
      return new type(value);
   }
   
   equals(id)
   {
      var bool =
         (
            (this.name == id.name) &&
            (this.ms == id.ms) &&
            (this.inc == id.inc)
           
         );
      return bool;
   }
   
}

Id.milliseconds = Date.now();
Id.increment = 0;
Id.types = new Map();

