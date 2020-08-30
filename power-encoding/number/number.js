Number.BASE = 5;
Number.PRECISION =
   1 / Math.pow(10, 5);
Number.LESS_ZERO    = 0 - Number.PRECISION;
Number.GREATER_ZERO = 0 + Number.PRECISION;

Number.prototype.encode = function(stream) {
   
   if (!stream)
      stream = new Stream();

   // x = (±)2^(±)n + r
   
   var x = this.valueOf();
    document.write("," + x);
   
   // by definition
   if (x >= Number.LESS_ZERO &&
       x <= Number.GREATER_ZERO )
   {
      stream.write("0");
      return stream;
   }
   /*
   if (x >= 1 + Number.LESS_ZERO &&
       x <= 1 + Number.GREATER_ZERO )
   {
      stream.write("1100");
      return stream;
   }
   if (x >= -1 + Number.LESS_ZERO &&
       x <= -1 + Number.GREATER_ZERO )
   {
      stream.write("1011");
      return stream;
   }
   */
   var numberSign;
   var exponentSign;
   var sign;
   const MINUS = "10";
   const PLUS = "11";
   
   
   // Sign of number
   if (x > 0) 
      numberSign = PLUS;
   else
   {
      numberSign = MINUS;
      x = -x;
   }

      
   // write the sign (and open a branch)
   stream.write(numberSign);
   if (numberSign === MINUS)
      stream.negate();

   
   // Sign of number
   if (x < 1)
   {
      exponentSign = MINUS;
      x = x * Math.pow(Number.BASE, -x.exponent);
   }
   else
   {
      exponentSign = PLUS;
   }

   
   // write the sign (and open a branch)
   stream.write(exponentSign);
   if (exponentSign === MINUS)
      stream.negate();

   // encode the exponent and
   // remainder branches
  
   var exponent = x.exponent;
   
   var power = Math.pow(
      Number.BASE,
      exponent
   );
   
   var remainder = x - power;
   
      
   exponent.encode(stream);   // branch
   remainder.encode(stream);  // branch
            
   return stream;
}

// get the highest integral
// exponent of a number
Object.defineProperty(
   Number.prototype,
   "exponent",
   {
      get: function() {
         var exponent =
            Math.log(this) /
            Math.log(Number.BASE);
       //  if (exponent >= 0)
            return Math.floor(exponent);
       //  else
        //    return Math.ceil(exponent);
      }
   }
);

Number.decode = function(
   stream
) {

   // bit equals "1"
   // open branch
   
   if (stream.peek() == "0")
   {
      stream.read();
      return 0;
   }
   
   var numberSign =
      stream.read() +
      stream.read();
   
   if (numberSign === "10")
   {
      numberSign = -1;
      stream.negate();
   }
   else if (numberSign === "11")
   {
      numberSign = 1;
   }
   
   if (stream.read() === "0")
      return 1 * numberSign;

   // get the exponent
   var exponent = Number.decode(
      stream
   );
   
   // get the remainder
   var remainder = Number.decode(
      stream
   );
   /*
   // calculate the power
   if (exponentSign == -1)
      exponent = 1 / exponent;

   var power = Math.pow(
      Number.BASE,
      exponent
   );
   */
   // calculate the number
   var x ;
   //= 
   //   numberSign * (power + remainder);
      
   return x;
}

