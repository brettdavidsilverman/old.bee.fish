class UInt extends UInt16 {

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
      if (UInt16.Cache[exponent]) {
         stream.write(
            UInt16.Cache[exponent]
         );
      }
      else
         exponent.encode(stream);
 
      // Encode the remainder
      var remainder = this.remainder(
         exponent
      );
     
      if (UInt16.Cache[remainder])
         stream.write(
            UInt16.Cache[remainder]
         );
      else
         remainder.encode(stream);
         
      return stream;
      
   }

   static decode(stream, type = UInt) {
   
      var value =
         UInt16.decode(stream, type);
      return value;
   }
   
};


