if (typeof UInt8 === "undefined")
   UInt8 = require("../uint8/uint8.js");

class UInt16 extends UInt8 {
      
   constructor(number) {
      super(number);
   }
   
   //           x
   //          / \
   // x === 2^n + r
   //
   // repeat for n (exponent)
   // and r (remainder)
   encode(stream = new Stream())
   {
         
      var x = this.valueOf();
      if (x <= UInt8.Cache.length)
      {
         stream.write(
            UInt8.Cache[x]
         );
         return stream;
      }
      
      // Open a new branch
      stream.write("1");
      
      // Encode this branch
      var exponent = this.exponent;
      if (exponent <= UInt8.Cache.length)
         stream.write(
            UInt8.Cache[exponent]
         );
      else
         exponent.encode(stream);
 
      // Encode the remainder
      var remainder = this.remainder(
         exponent
      );
     
      if (remainder < UInt8.Cache.length)
         stream.write(
            UInt8.Cache[remainder]
         );
      else
         remainder.encode(stream);
         
      return stream;
      
   }
   
   static decode(stream, type = UInt16) {
      var value = 
         UInt8.decode(stream, type);
      return value;
   }
   
};

UInt16.BASE = UInt8.BASE;
UInt16.MAX = Math.pow(2, 16);


module.exports = UInt16;

