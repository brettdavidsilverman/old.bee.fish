class Id {
   // Creates a value that is
   // guaranteed yo be unique.
   // The value is milliseconds
   // and an incrementrement for objects
   // created close in time 
   // to each other.
   // The incrementrement is reset when
   // the milliseconds ticks over
   
   constructor(input = {key:null}) {
   
      Object.assign(this, input);
      var key = input.key;
      
      if (key)
         this.key = key;
      else {
         var milliseconds = Date.now();
      
         if ( milliseconds ==
              Id.milliseconds )
            Id.increment++;
         else {
            Id.increment = 0;
            Id.milliseconds = milliseconds;
         }
   
         var increment = Id.increment;
        
         this[Id.timestamp] = {
            milliseconds,
            increment
         }
      }
   }
    
   set key(key) {
      
      var bitString =
         BitString
         .fromUnicodeString(key);
         
      var bits = bitString.bits;
      
      var index = new Number.Index(1);
      var milliseconds =
         Number.decode(bits, index);
      var increment =
         Number.decode(bits, index);
               
      this[Id.time] = {
         milliseconds,
         increment
      }
      
      this[Id.key] = key;
   }


   get key() {
      
      if (!this[Id.key])
         this[Id.key] = createKey(this);
      
      return this[Id.key];
      
      function createKey(id) {
      
         var timestamp = id[Id.timestamp];
         
         var bits = new BitString(
            "1" + // open branch
            timestamp.milliseconds.encode() +
            timestamp.increment.encode()
         );
 
         var key =
            bits.toUnicodeString();
      
         return key;
      }
 
   }
   
   lessThenOrEqual(timestamp) {
   
      var ts = this[Id.timestamp];
      
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

   
   static isId(object) {
      var isId =
         object &&
         object[Id.timestamp];
      return isId;
   }
 
}

Id.milliseconds = Date.now();
Id.increment = 0;
Id.key = Symbol("key");
Id.timestamp = Symbol("timestamp");

class BitString {

   constructor(bits) {
      this.bits = bits
   }
   
   toString(characterBitLength = 16) {

      var string = "";
      
      for ( var index = 0;
            index < this.bits.length;
            index +=
               characterBitLength )
      {
  
         // get character size
         // number of bits
         var charBits =
            this.bits.substr(
               index,
               characterBitLength
            );
         
         // Convert bit string to number
         var charCode = parseInt(
            charBits, 2
         );
         
         // Convert number to character
         // and append to strint
         string += String.fromCharCode(
            charCode
         );
      }
     
      return string;
   }
   
   toCharString() {
      return this.toString(8);
   }
   
   toUnicodeString() {
      return this.toString(16);
   }
   
   static fromUnicodeString(string) {
      return
         BitString.fromString(
            string,
            16
         );
   }
   
   static fromCharString(string) {
      return
         BitString.fromString(
            string,
            8
         );
   }
   
   static fromString(
      string,
      characterBitLength = 16)
   {
      var bits = "";
      for ( var index = 0;
            index < string.length;
            index++ ) 
      {
         var charCode =
            string.charCodeAt(
               index * characterBitLength
            );
         bits += charCode.toString(2);
      }
      
      return new BitString(bits);
   }
}



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

