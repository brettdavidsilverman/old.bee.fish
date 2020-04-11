class Id  {

  
   static ms = Date.now();
   static inc = 0;
 
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
      if (this.#key === undefined)
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
      
      timestamp.ms.encode(stream);
      timestamp.inc.encode(stream);
         
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
      if (this.#timestamp === undefined) {
         this.#timestamp =
            this.extractTimestamp();
      }
      return this.#timestamp.ms;
   }
   
   get inc() {
      if (this.#timestamp === undefined)
         this.#timestamp =
            this.extractTimestamp();

      return this.#timestamp.inc;
   }
   
   extractTimestamp() {
   
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
         ms: Number.decode(stream),
         inc: Number.decode(stream)
      }

   }
 
   toJSON() {
      var output;
      switch (Shorthand.type)
      {
      case Shorthand.HUMAN:
         output = this.name;
         break;
      case Shorthand.FULL:
         output = {
            name: this.name,
            ms: this.ms,
            inc: this.inc,
            key:  this.key
         };
         break;
      case Shorthand.POINTERS:
         output = {
            name: this.name,
            key:  this.key
         };
         break;
      default:
         output = {
            name: this.name,
            ms: this.ms,
            inc: this.inc
         };
      }
    
      return output;
   }
   
   toString() {
      new Shorthand(Shorthand.HUMAN);
      return JSON.stringify(
         this,
         null,
         "   "
      );
      Shorthand.pop();
   }
   
   get name() {
      return this.#name;
   }
   
   get typeFunction() {
   
      return getTypeFunction(this.#name);
      
      function getTypeFunction(name) {
         var f = new Function(
            "return " + name + ";"
         );
         return f();
      }
      
   }
   
}


Object.defineProperty(
   Object.prototype,
   "=",
   {
      get: getId,
      set: setId,
      enumerable:   true,
      configurable: true
   }
);

function getId() {
   var id;
   if (this instanceof Id)
      id = null;
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



