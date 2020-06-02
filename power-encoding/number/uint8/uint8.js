if (typeof Stream === "undefined")
   Stream = require("../../stream/stream.js");

class UInt8 extends Number {
   
   constructor(number) {
      super(number);
   }
   
   //           x
   //          / \
   // x === 2^n + r
   //
   // repeat for n (exponent)
   // and r (remainder)
   encode(stream = new Stream()) {
         
      var x = this.valueOf();

      // Define zero as "0"
      if (x === 0) {
         stream.write("0");
         return stream;
      }
      
      // Define 1 as...
      //
      //            1
      //           / \
      // One is 2^0 + 0
      if (x === 1) {
         stream.write("100");
         return stream;
      }
      
      // Define 2 as...
      //
      //     2
      //    / \
      //   1   0
      //  /\  
      // 0  0   
      if (x === 2) {
         stream.write("11000");
         return stream;
      }

      // Open a new branch
      stream.write("1");
      
      // Encode this branch
      var exponent = this.exponent;
      exponent.encode(stream);
 
      // Encode the remainder
      var remainder = this.remainder(
         exponent
      );
      remainder.encode(stream);
      
      return stream;
      
   }
   
   static decode(stream, type = UInt8) {
   
      var bit = stream.read();
      
      if ( bit === "0" ) {
         return 0;
      }
      
      var exponent =
         Number(
            type.decode(stream, type)
         );
         
      var remainder =
         Number(
            type.decode(stream, type)
         );
         
      var value = new type(
         Math.pow(
            UInt8.BASE, exponent
         ) +
         remainder
      );
      
      return value;
   }
   
   get exponent() {
      return new UInt8(
         Math.floor(
            Math.log(this) /
            Math.log(UInt8.BASE)
         )
      );
   }
   
   remainder(exponent) {
      return new UInt8(
         this -
         Math.pow(UInt8.BASE, exponent)
      );
   }
   
   toString() {
      var value = Number(this).toString();
      return value;
   }
   
};

UInt8.BASE = 2;
UInt8.MAX = 255;

UInt8.Cache = new Array(256);

for (i = 0; i < 256; ++i) {
   var x = new UInt8(i);
   var stream = x.encode();
   UInt8.Cache[i] = stream.bits;
}

module.exports = UInt8;


