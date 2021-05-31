class Id  {
 
   time = undefined;
   increment  = undefined;
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
             ( this.time == undefined ||
               this.increment  == undefined ||
               this.name == undefined ) )
         this._decodeKey();
      else if ( this.time == undefined ||
                this.increment == undefined )
         this._createTimestamp();
      
      if ( this.name == undefined )
      {
         throw new Error("Missing name");
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
      var time = Date.now();
      
      if ( time === Id.time )
         ++Id.increment;
      else {
         Id.increment = 0;
         Id.time = time;
      }
   
      this.time = time;
      this.increment = Id.increment;

   }

   _createKey()
   {

      if ( this.time == undefined ||
           this.increment == undefined )
         this._createTimestamp();
         
      var stream = new PowerEncoding();
      
      // Start bit
      stream.write("1");
      
      // encode name
      this.name.encode(stream);
      
      // encode time
      this.time.encode(stream);
      
      // encode incrementrement
      this.increment.encode(stream);
      
      // end bit
      stream.write("0");
      
      var data = stream.toString();
      
      var key = data;
      
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
      var data = this.key;
      
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
      this.time = Number.decode(stream);
      
      // read the incrementrement
      this.increment = Number.decode(stream);
      
      CHECK(
         "Id._decodeKey end bit",
         stream.read() == "0"
      );
      
      CHECK(
         "Id._decodeKey count",
         stream.count == 0
      );


   }
   
   toShorthand(shorthand)
   {
      var output;
      var time, increment, key, name;
      
      if (shorthand & Shorthand.HUMAN)
         name = this.name;
      
      if (shorthand & Shorthand.FULL)
      {
         name = this.name;
         time = this.time;
         increment = this.increment;
      }
      
      if (shorthand & Shorthand.COMPUTER)
      {
         key = this.key;
         name = this.name;
      }
            
      return {
         name,
         key,
         time,
         increment
      }
      
   }

   toString(shorthand = Shorthand.HUMAN)
   {
      var object =
         this.toShorthand(shorthand);
      
      var string = JSON.stringify(
         object,
         null,
         "   "
      );
 
      return string;
   }
   
   get type() {
   
      if (Id.types[this.name])
         return Id.types[this.name];
         
         
      Id.types[this.name] =
         getType(this.name);
      
      return Id.types[this.name];
      
      function getType(name) {
         var f = new Function(
            "return " + name + ";"
         );
         return f();
      }
      
   }
   
   equals(id)
   {
      var bool =
         (
            (this.name == id.name) &&
            (this.time == id.time) &&
            (this.increment == id.increment)
           
         );
      return bool;
   }
   
}

Id.time = Date.now();
Id.increment = 0;
Id.types = new Map();

defineId(Object);
defineId(Array);
defineId(Function);

function defineId(type) {
   Object.defineProperty(
      type.prototype,
       "=",
      {
         get: getId,
         set: setId,
         enumerable:   false,
         configurable: true
      }
   );
}

function getId() {
   var id;
   if (this instanceof Id) {
      id = null;
   }
   else {
      id = new Id(
         {
            name: this.constructor.name
         }
      );
   }

   this["="] = id;
  
   return id;
}

function setId(id) {
      
   Object.defineProperty(
      this,
      "=",
      {
         value: id,
         writable: true,
         enumerable: true,
         configurable: false
      }
   );
   
}
