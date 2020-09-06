Number.BASE = 5;
Number.PRECISION = 100
Number.LESS_ZERO    = - Math.pow(10, -Number.PRECISION);
Number.GREATER_ZERO = + Math.pow(10, -Number.PRECISION);
const MINUS = "01";
const PLUS = "10";

Number.prototype.encode = function(stream) {
   
   if (!stream)
      stream = new Stream();

   // x = (±)2^(±)n + r
   
   var x = this.valueOf();
  //  document.write("," + x);
   
   // by definition
   if (x >= Number.LESS_ZERO &&
       x <= Number.GREATER_ZERO )
   {
      stream.write("0");
      return stream;
   }

   stream.write("1");
   
   var numberSign;
   var exponentSign;
   
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
      x = 1 / x;
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
   
   if (stream.read() == "0")
   {
      return 0;
   }
   
   
   // Read the overall number sign
   var numberSign;
   
   if (stream.read()  === "0")
   {
      numberSign = -1;
      stream.negate();
   }
   else
   {
      numberSign = 1;
   }
   
   stream.read();
   
   // Read the exponent sign
   var inverse;
   
   if (stream.read() === "0")
   {
      inverse = true;
      stream.negate();
   }
   else
      inverse = false;
      
   stream.read();

   // get the exponent
   var exponent = Number.decode(
      stream
   );
   
   // get the remainder
   var remainder = Number.decode(
      stream
   );
   

   var power = Math.pow(
      Number.BASE,
      exponent
   );
   
   // calculate the number
   var number =
      numberSign * (power + remainder);
      
   // calculate the invrtse
   if (inverse)
      number = 1 / number;
      
   return number;
}

