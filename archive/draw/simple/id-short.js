class Id {
   // Creates a value that is
   // guaranteed yo be unique.
   // The value is milliseconds (ms)
   // and an increment (inc) for objects
   // created close in time 
   // to each other.
   // The increment is reset when
   // the milliseconds ticks over
   
   // Id also inludes a name
   // which is the name of the objects
   // constructor
   // This name is set by the Storable
   // class
   constructor(input = {}) {
      /*
      this.ms =
         input.ms;
      if ( !this.ms )
         this.ms = Date.now();
      
      if ( this.ms ==
           Id.ms )
      {
         Id.inc++;
      }
      else {
         Id.inc = 0;
         Id.ms = this.ms;
      }
   
      this.inc = input.inc;
      if (this.inc == undefined)
         this.inc = Id.inc;
      */
      this.i = input.i;
      if (this.i == undefined)
         this.i = Id.i++;
         
      this.name = input.name;
      this.key = input.key;

      if (!(this.key)) {
         Object.defineProperty(
            this,
            "key",
            {
               get: function() {
   
                  var key =
                     getKey(this);
      
                  this.key = key;
      
                  return key;
               },
   
               set: function(value) {

                  Object.defineProperty(
                     this,
                     "key",
                     {
                        value: value,
                        enumerable: true
                     }
                  );
      
               },
               enumerable: true,
               configurable: true
            }
         );
      }
      
      function getKey(id) {
      /*
         var bits = new BitString(
            "1" + // open branch
            id.ms.encode() +
            id.inc.encode()
         );
       */
         var bits = new BitString(
            id.i.encode()
         );
         
         var key =
            bits.toUnicodeString();
      
         return key;
      }
 
   }
   
   lessThenOrEqual(timestamp) {
   
      return (this.i < timestamp.i);
      /*
      if (this.ms <
          timestamp.ms)
         return true;
      else if (this.ms ==
               timestamp.ms)
         return this.inc <=
                timestamp.inc;
      else
         return false;
      */
   }
   
   createBuilder() {
      var builder = new Function(
         "return " + this.name + ";"
      );
      return builder();
   }
 
}
/*
Id.ms = Date.now();
Id.inc = 0;
*/
Id.i = 0;

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

