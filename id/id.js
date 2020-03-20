function Id(
      input
   )
{
   // Creates a value that is
   // guaranteed yo be unique.
   // The value is milliseconds
   // and an increment for objects
   // created close in time 
   // to each other.
   // The increment is reset when
   // the milliseconds ticks over
   
   function createTimestamp() {
      // create a new timestamp
      var milliseconds = Date.now();
      
      if ( milliseconds ==
           Id.milliseconds )
         ++Id.increment;
      else {
         Id.increment = 0;
         Id.milliseconds = milliseconds;
      }
   
      var increment = Id.increment;
        
      return {
         milliseconds: milliseconds,
         increment: increment
      }
      
   }

   this.getKey = function() {
      
      if (this.timestamp == undefined)
         return undefined;
         
      this.key =
         createKey(this.timestamp);
      
      return this.key;
     
   }
   
   function setKey(key) {
       
      Object.defineProperty(
         this,
         "key",
         {
            value: key,
            writable: false,
            configurable: false
         }
      );
      
   }

   Object.defineProperty(
      this,
      "key",
      {
         get: this.getKey,
         set: setKey,
         configurable: true
      }
   );
   
   this.getTimestamp = function() {
   
      if (this.key == undefined)
         return undefined;
         
      this.timestamp = 
         extractTimestamp(this.key);
      
      return this.timestamp;
   }
   
   function setTimestamp(value) {
      Object.defineProperty(
         this,
         "timestamp",
         {
            value: value,
            writable: false,
            configurable: false,
            enumerable: true
         }
      );
   }
   
   Object.defineProperty(
      this,
      "timestamp",
      {
         get: this.getTimestamp,
         set: setTimestamp,
         configurable: true,
         enumerable: true
      }
   );
   
   function createKey(timestamp) {
      
      var stream = new Stream();

      // encode timestamp
      stream.write("1");
      timestamp.milliseconds.encode(stream);
      timestamp.increment.encode(stream);
         
      var bitString = new BitString(
         {
            bits: stream.bits
         }
      );
         
      var key =
         bitString.toString();
      
      return key;
   }
 
   function extractTimestamp(key) {
      // extract the timestamp
      // from the key
      var bitString =
         BitString
         .fromUnicodeString(key);
         
      var stream = new Stream(bitString.bits);
      
      stream.read();
      
      var milliseconds =
         Number.decode(stream);
   
      var increment =
         Number.decode(stream);
      
      return {
         milliseconds: milliseconds,
         increment: increment
      }

   }

      
   this.lessThenOrEqual = function
      (timestamp)
   {
   
      var ts = this._timestamp;
      
      if (ts.milliseconds <
          timestamp.milliseconds)
         return true;
      else if (ts.milliseconds ==
               ts.milliseconds)
         return ts.increment <=
                timestamp.increment;
      else
         return false;
         
   }

  
 
   this.toObject = function(input) {

      if (input.shortHand == "short")
         return this.name;
      else if (input.shortHand == "pointers") {
         return this.name;
      }
      else if (input.shortHand == "full") {
       
         var timestamp = this.timestamp;
         return {
            milliseconds: timestamp.milliseconds,
            increment:    timestamp.increment,
            key:          this.key
         }
      }

   }
   
   
   if (!input)
      input = {}
      

   if (input.key) {
      // The key compromised of encoded
      // time: milliseconds and increment
      this.key = input.key;
      // an optional name, usually the hosts
      // constructors name
      this.name = input.name;
   }
   else {
      // Create a new timestamp
      this.timestamp = createTimestamp();
   }
   
   
}

Id.isId = function(object) {

  var isId = (
     ( (object) &&
       (object.constructor == Id) ) ||
     ( (object.key          != null) &&
       (object.milliseconds != null) &&
       (object.increment    != null) )
  );
  
  return isId;
}

Id.milliseconds = Date.now();
Id.increment = 0;

// Create the id for the first
// time on this object.
// Set the property such that
// the id is fixed.
function getId() {
      
   // Create the id
   var id = new Id();
   this.id = id;
   this.name = this.constructor.name;
   return id;
   
}

// Replace the get/set with a
// fixed value
function setId(value) {
   Object.defineProperty(
      this,
      "id",
      {
         value: value,
         enumerable: true,
         writable: false
      }
   );
}

// Gets or creates a new identifier
// Sets the id property
Object.defineProperty(
   Object.prototype,
   "id",
   {
      get: getId,
      set: setId,
      enumerable: true
   }
);


