class Id  {
 
   #timestamp = undefined;
   #key = undefined;
   #name = undefined;
   #hex = undefined;
   
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
      else if ( input && input.hex )
      {
         // The key comprised of hex values
         this.#hex = input.hex;
      }
      else
      {
         // Create a new timestamp
         this.#timestamp =
            this.createTimestamp();
      }
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
      
      var key;
      
      if (this.#timestamp)
      {
         key =
            this.createKeyFromTimestamp(
               this.#timestamp
            );
      }
      else if (this.#hex)
      {
         key =
            this.createKeyFromHex(this.#hex);
      }
      else
         throw "Not enough info to create key";
         
      
      return key;
   }
   
   createKeyFromTimestamp(timestamp)
   {

      var stream = new Stream();
      
      // encode timestamp
      stream.write("1");
      
      var milliseconds =
         new UInt(timestamp.ms);
      stream.write("1");
      milliseconds.encode(stream);
      
      var increment =
         new UInt(timestamp.inc);
      stream.write("1");
      increment.encode(stream);
         
      stream.write("0");
      
      /*
      var bits = stream.bits;
      var count = 0;
      for (var i = 0; i < bits.length; ++i)
      {
         if (bits[i] == '0')
            --count;
         else
            ++count;
      }
      
      document.writeln(count);
      */
      
      var bitString = new BitString(
         {
            bits: stream.bits
         }
      );

      var key =
         bitString.toString();
         
      return key;
         
   }
      
   createKeyFromHex(hex)
   {
      var key = "";
      for (var i = 0; i < hex.length; i += 4)
      {
         var hexUpper = hex.substr(i, 2);
         var hexLower = hex.substr(i + 2, 2);
         var upper = parseInt(hexUpper, 16);
         var lower = parseInt(hexLower, 16);
         var number = (upper << 8) | lower;
         key += String.fromCharCode(number);
      }
         
      return key;
      
   }
      
 
   get ms() {
      if (!this.#timestamp) {
         this.#timestamp =
            this.createTimestampFromKey(
               this.key
            );
      }
      return this.#timestamp.ms;
   }
   
   get inc() {
      if (!this.#timestamp)
         this.#timestamp =
            this.createTimestampFromKey(
               this.key
            );

      return this.#timestamp.inc;
   }
   
   get timestamp() {
      if (!this.#timestamp)
         this.#timestamp = 
            this.createTimestampFromKey(
               this.key
            );
      return this.#timestamp;
   }
   
   set timestamp(value) {
      this.#timestamp = value;
   }
   
   createTimestampFromKey(key) {
   
      // extract the timestamp
      // from the key
      var bitString =
         BitString
         .fromUnicodeString(key);
         
      var stream = new Stream(bitString.bits);
      
      // read the first "1"
      CHECK(stream.read() == '1');
      
      // read 1 for ms
      CHECK(stream.read() == '1');
      var ms = Number(UInt.decode(stream));
      
      // read 1 for inc
      CHECK(stream.read() == '1');
      var inc = Number(UInt.decode(stream));
      
      // read 0
      CHECK(stream.read() == '0');
      
      return {
         ms: ms,
         inc: inc
      }
      
      function CHECK(bool)
      {
         if (bool == false)
            throw "Check failed"
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
   
   get hex() {
      
      if (!this.#hex)
         this.#hex = createHexFromKey(this.key);
         
      return this.#hex;
      
      function createHexFromKey(key)
      {
         var hexString = "";
      
         for (var i = 0; i < key.length; ++i) {
            var number = key.charCodeAt(i);
            var high = (number & 0xFF00) >> 8;
            var low = number & 0x00FF;

            hexString +=
               toHex(high) + toHex(low);
            
         }
      
         return hexString;
      
         function toHex(number)
         {
            return number
               .toString(16)
               .toUpperCase()
               .padStart(2, "0");
         }
         
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
