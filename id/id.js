class Id  {
 
   #timestamp = undefined;
   #key = undefined;
   #name = undefined;
   
   // Creates a value that is
   // guaranteed to be unique
   // from this device.
   // The value is milliseconds (ms)
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
         this.#name = input;
      else if (input && input.name)
         // Copy constructor
         this.#name = input.name;
         
      // Either set the key or
      // set the timestamp.
      if ( input && input.key )
         // The key compromised of encoded
         // time: milliseconds and
         // increment
         this.#key = input.key;
      else if ( input && input.timestamp )
         this.#timestamp = input.timestamp;
      else
         // Create a new timestamp
         this.#timestamp =
            this.createTimestamp();
   }
   
   createTimestamp() {
      // create a new timestamp
      var ms = Date.now();
      
      if ( ms === Id.ms )
         ++Id.inc;
      else {
         Id.inc = 0;
         Id.ms = ms;
      }
   
      var inc = Id.inc;
        
      return {
         ms,
         inc
      }
   }

   get key() {
   
      if (!this.#key)
         this.#key = this.createKey();
         
      return this.#key;
     
   }
   
   set key(value) {
   }
   

   createKey() {
      
      var timestamp = this.#timestamp;
      var stream = new Stream();
      
      // encode timestamp
      stream.write("1");
      
      var milliseconds =
          new UInt(timestamp.ms);
      milliseconds.encode(stream);
      
      var increment =
         new UInt(timestamp.inc);
      increment.encode(stream);
         
      var bitString = new BitString(
         {
            bits: stream.bits
         }
      );
         
      var key =
         bitString.toString();
         
      return key;
   }
 
   get ms() {
      if (!this.#timestamp) {
         this.#timestamp =
            this.decodeTimestamp();
      }
      return this.#timestamp.ms;
   }
   
   get inc() {
      if (!this.#timestamp)
         this.#timestamp =
            this.decodeTimestamp();

      return this.#timestamp.inc;
   }
   
   get timestamp() {
      return {
         ms: this.ms,
         inc: this.inc
      }
   }
   
   set timestamp(value) {
      this.#timestamp = value;
      this.#key = null;
   }
   
   decodeTimestamp() {
   
      var key = this.#key;
      
      // extract the timestamp
      // from the key
      var bitString =
         BitString
         .fromUnicodeString(key);
         
      var stream = new Stream(bitString.bits);
      
      // read the first "1"
      stream.read();
      
      return {
         ms: Number(UInt.decode(stream)),
         inc: Number(UInt.decode(stream))
      }

   }
 
   toShorthand(shorthand) {
      var output;
      var name, key, timestamp;
      
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
            timestamp,
            key
         }
      }

      return output;
   }

   get name() {
      return this.#name;
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

Id.ms = Date.now();
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



