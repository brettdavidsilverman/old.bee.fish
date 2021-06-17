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
   
      
      if ( typeof input == "string")
         this.name = input;
      else
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
                  var key = this._createKey();
                  return key;
               },
               enumerable: true,
               configurable: true
            }
         );
      }

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

   _createKey()
   {

      if ( this.ms == undefined ||
           this.inc == undefined )
         this._createTimestamp();
         
      var stream = new PowerEncoding();
      
      // Start bit
      stream.write("1");
      
      // encode name
      //this.name.encode(stream);
      
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
         this._createKey();
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
      //this.name = String.decode(stream);

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
   
   get type() {
   
      if (Id.types[this.name])
         return Id.types[this.name];
         
         
      Id.types[this.name] =
         getType(this.name);
      
      return Id.types[this.name];
      
      function getType(name) {
         console.log("Be aware of injection scripts here");
         var f = new Function(
            "return " + name + ";"
         );
         return f();
      }
      
   }
   
   toJSON() {
      return this;
   }
   
   save() {
      var value = this.toJSON();
      return storage.setItem(this.key, value);
   }
   
   static async load(type, key) {
      var value = await storage.getItem(key);
      if (value == undefined)
         return null;
      value.key = key;
      var object = new type(value);
      return object;
   }
   
   equals(id)
   {
      var bool =
         (
            (this.name == id.name) &&
            (this.ms == id.time) &&
            (this.inc == id.increment)
           
         );
      return bool;
   }
   
}

Id.milliseconds = Date.now();
Id.increment = 0;
Id.types = new Map();

