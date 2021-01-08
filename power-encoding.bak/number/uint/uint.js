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
      
      if (x < UInt16.Cache.length) {
         stream.write(
            UInt16.Cache[x]
         );
         return stream;
      }
      
      // Open a new branch
      stream.write("1");
      
      // Encode this branch
      var exponent = this.exponent;
      if (exponent >= 0 && exponent < UInt16.Cache.length) {
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
     
      if (remainder >= 0 && remainder < UInt16.Cache.length)
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


