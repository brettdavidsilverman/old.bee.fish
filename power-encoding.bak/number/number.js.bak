
Number.BASE = 5;
Number.PRECISION =
   1 / Math.pow(10, 6);
Number.LESS_ZERO    = 0 - Number.PRECISION;
Number.GREATER_ZERO = 0 + Number.PRECISION;

Number.prototype.encode = function(stream) {
   
   if (!stream)
      stream = new Stream();

   // x = (±)2^(±)n + r
   
   var x = this.valueOf();
   
   // by definition
   if (x >= Number.LESS_ZERO &&
       x <= Number.GREATER_ZERO ) {
      stream.write(Number.ZERO);
      return stream;
   }
   
   var numberSign;
   var exponentSign;
   var sign;
   const PLUS = 1;
   const MINUS = -1;
   
   // Sign of number
   if (x > 0) 
      numberSign = PLUS
   else {
      numberSign = MINUS;
      x = -x;
   }
   
   // sign of exponent
   if (x >= 1)
      exponentSign = PLUS;
   else {
      exponentSign = MINUS;
     // x = 1 / x;
   }
   
   // cross join both signs combinations
   if (numberSign === PLUS &&
       exponentSign === PLUS)
       
      sign = Number.PLUS_PLUS;
      
   else if (numberSign === PLUS &&
            exponentSign === MINUS)
            
      sign = Number.PLUS_MINUS;
      
   else if (numberSign === MINUS &&
            exponentSign === PLUS)
            
      sign = Number.MINUS_PLUS;
      
   else if (numberSign === MINUS &&
            exponentSign === MINUS)
            
      sign = Number.MINUS_MINUS;
      
   else
   
      throw new Error(
         "Invalid sign whilst encoding Number"
      );
      
   // write the sign (and open a branch)
   stream.write(sign);
   
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
         if (exponent > 0)
            return Math.floor(exponent);
         else
            return Math.ceil(exponent);
      }
   }
);

// number, exponent  Name  Order
// +       +         P_P   5
// +       -         P_M   4
// zero              Z     3
// -       -         M_M   2
// -       +         M_P   1

Number.decode = function(
   stream
) {

   // bit equals "1"
   // open branch
   
   var sign = UInt8.decode(stream);
  
   var numberSign;
   var exponentSign;
   
   switch (sign.valueOf()) {
   case 1:
      numberSign = -1;
      exponentSign = 1;
      break;
   case 2:
      numberSign = -1;
      exponentSign = -1;
      break;
   case 3:
      return 0;
   case 4:
      numberSign = 1;
      exponentSign = -1;
      break;
   case 5:
      numberSign = 1;
      exponentSign = 1;
      break;
   default:
      throw new Error(
         "Invalid sign {" + sign + "}"
      );
   }
   
  

   // get the exponent
   var exponent = Number.decode(
      stream
   );
   
   // get the remainder
   var remainder = Number.decode(
      stream
   );
   
   // calculate the power
   if (exponentSign == -1)
      exponent = 1 / exponent;
      
   var power = Math.pow(
      Number.BASE,
      exponent
   );
   
   // calculate the number
   var x = 
      numberSign * (power + remainder);
      
   return x;
}


// number, exponent  Name  Order
// +       +         P_P   5
// +       -         P_M   4
// zero              Z     3
// -       -         M_M   2
// -       +         M_P   1
Number.PLUS_PLUS   = UInt8.Cache[5];
Number.PLUS_MINUS  = UInt8.Cache[4];
Number.ZERO        = UInt8.Cache[3];
Number.MINUS_MINUS = UInt8.Cache[2];
Number.MINUS_PLUS  = UInt8.Cache[1];