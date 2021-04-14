class Id  {
 
   _timestamp = undefined;
   _key = undefined;
   _name = undefined;
   
   // Creates a value that is
   // guaranteed to be unique
   // from this device.
   // The value is milliseconds (time)
   // and an increment (inc)
   // for objects created close
   // in time to each other.
   // The increment is reset when
   // the milliseconds ticks over
   constructor(input) {
      
      // Make sure we have a name
      if (typeof(input) === "string")
         // Usually the parents
         // constructors name
         this._name = input;
      else if (input && input.name)
         // Copy constructor
         this._name = input.name;
         
      // Either set the key or
      // set the timestamp.
      if ( input && input.key )
         // The key compromised of encoded
         // time: milliseconds and
         // increment
         this._key = input.key;
      else if ( input && input.timestamp )
         this._timestamp = input.timestamp;
      else
      {
         // Create a new timestamp
         this._timestamp =
            this.createTimestamp();
      }
   }
   
   createTimestamp() {
      // create a new timestamp
      var time = Date.now();
      
      if ( time === Id.time )
         ++Id.inc;
      else {
         Id.inc = 0;
         Id.time = time;
      }
   
      var inc = Id.inc;
        
      return {
         time,
         inc
      }
   }

   get key() {
   
      if (!this._key)
         this._key =
            this.createKeyFromTimestamp(
               this._timestamp
            );
      return this._key;
     
   }
   
   set key(value) {
   }
   

   createKeyFromTimestamp(timestamp)
   {

      var stream = new Stream();
      
      // encode timestamp
      stream.write("1");
      
      var milliseconds =
         new PowerEncoding(timestamp.time);
      stream.write("1");
      milliseconds.encode(stream);
      
      var increment =
         new PowerEncoding(timestamp.inc);
      stream.write("1");
      increment.encode(stream);
         
      stream.write("0");
      
      return stream.base64;
         
   }
      
   createTimestampFromKey(key) {
   
      // extract the timestamp
      // from the key

      var stream =
         Stream.fromBase64(key);
      
      // read the first "1"
      CHECK(stream.read() == '1');
      
      // read 1 for time
      CHECK(stream.read() == '1');
      var time = PowerEncoding.decode(stream);
      
      // read 1 for inc
      CHECK(stream.read() == '1');
      var inc = PowerEncoding.decode(stream);
      
      // read 0
      CHECK(stream.read() == '0');
      
      return {
         time: Number(time),
         inc: Number(inc)
      }
      
      function CHECK(bool)
      {
         if (bool == false)
            throw "Check failed"
      }

   }
   
   get time() {
      if (!this._timestamp) {
         this._timestamp =
            this.createTimestampFromKey(
               this.key
            );
      }
      return this._timestamp.time;
   }
   
   get inc() {
      if (!this._timestamp)
         this._timestamp =
            this.createTimestampFromKey(
               this.key
            );

      return this._timestamp.inc;
   }
   
   get timestamp() {
      if (!this._timestamp)
         this._timestamp = 
            this.createTimestampFromKey(
               this._key
            );
      return this._timestamp;
   }
   
   set timestamp(value) {
      this._timestamp = value;
   }
   
   
 
   toShorthand(shorthand) {
      var output;
      var name, timestamp;
      
      if (shorthand & Shorthand.HUMAN)
         output = this.name;
      else {
         if (shorthand & Shorthand.FULL) {
            name = this.name;
            timestamp = this.timestamp;
         }
         if (shorthand & Shorthand.COMPUTER) {
            name = this.name;
         }
            
         output = {
            name,
            timestamp
         }
      }

      return output;
   }

   get name() {
      return this._name;
   }
   
   get Type() {
   
      if (Id.Types[this.name])
         return Id.Types[this.name];
         
      var type =
         getType(this.name);
         
      Id.Types[this.name] = type;
      
      return type;
      
      function getType(name) {
         var f = new Function(
            "return " + name + ";"
         );
         return f();
      }
      
   }
   
   
}

Id.time = Date.now();
Id.inc = 0;
Id.Types = new Map();

function defineId(Type) {
   Object.defineProperty(
      Type.prototype,
       "=",
      {
         get: getId,
         set: setId,
         enumerable:   false,
         configurable: true
      }
   );
}

defineId(Object);
defineId(Array);
defineId(Function);

function getId() {
   var id;
   if (this instanceof Id) {
      id = null;
   }
   else {
      id = new Id(
         this.constructor.name
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
         writable: false,
         enumerable: true,
         configurable: false
      }
   );
   
}
