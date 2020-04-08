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
      
      if (typeof(input) === "string")
         // a name, usually the hosts
         // constructors name
         this.#name = input;
      else if (input && input.name)
         // copy constructor
         this.#name = input.name;
         
      if ( input === undefined ||
           input === null ||
           input.key === undefined ) {
         // Create a new timestamp
         this.#timestamp =
            this.createTimestamp();
      }
      else {
         // The key compromised of encoded
         // time: ms and inc
         this.#key = input.key;
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
         ms: ms,
         inc: inc
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
      var shortHand = ShortHand.current;
     
      switch (ShortHand.current)
      {
      case ShortHand.human:
         output = this.name;
         break;
      case ShortHand.full:
         output = {
            name: this.name,
            ms: this.ms,
            inc: this.inc,
            key:  this.key
         };
         break;
      case ShortHand.state:
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
   
   toString(shortHand = ShortHand.human) {
      ShortHand.current = shortHand;
      return JSON.stringify(
         this,
         null,
         "   "
      );
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
   
   static checkId(object) {

      var id;
      if ("=" in object) {
         id = object["="];
         if (!(id instanceof Id)) {
            id = new Id(id);
            object["="] = id;
         }
      }
      else
      {
         id = new Id(
            object.constructor.name
         );
         object["="] = id;
      }
      
      return id;
   }
  
   
}



