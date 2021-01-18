class PowerEncoding extends Number {

   static BASE = 10;
   
   constructor(number) {
      super(number);
   }
   
   //           x
   //          / \
   // x === 5^n + r
   //
   // repeat for n (exponent)
   // and r (remainder)
   encode(stream = new Stream()) {
         
      if (isNaN(this)) {
         var error =
            "PowerEncoding: Not a number: " +
            String(this);
         throw error;
      }
         
      var x = Number(this);

      // Define zero as "0"
      if (x === 0) {
         stream.write("0");
         return stream;
      }

      // (presuming base 5)...
      //
      // Define 1 as...
      //
      //            1
      //           / \
      // One is 5^0 + 0
  
      
      // So 5 is...
      //
      //     5
      //    / \
      //   1   0
      //  /\  
      // 0  0   

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
   
   static decode(stream)
   {
   
      var bit = stream.read();
      
      if ( bit === "0" )
      {
         return 0;
      }
      
      var exponent =
         PowerEncoding.decode(stream);
         
      var remainder =
         PowerEncoding.decode(stream);
         
      var value = new PowerEncoding(
         Math.pow(
            PowerEncoding.BASE, exponent
         ) +
         remainder
      );
      
      return value;
   }
   
   get exponent() {
      return new PowerEncoding(
         Math.floor(
            Math.log(Number(this)) /
            Math.log(PowerEncoding.BASE)
         )
      );
   }
   
   remainder(exponent) {
      return new PowerEncoding(
         Number(this) -
         Math.pow(PowerEncoding.BASE, exponent)
      );
   }
   
   toString() {
      var value = Number(this).toString();
      return value;
   }
   
};


