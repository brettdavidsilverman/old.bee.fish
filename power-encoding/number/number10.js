Number.BASE = 5;
Number.PRECISION = Math.pow(10, 5);

Number.prototype.encode = function(stream) {
   
   if (!stream)
      stream = new Stream();

   var number = this.valueOf();
   document.write(number + ",");
   
   if (number < 0)
   {
      stream.write("-");
      number = -number;
   }
   
   number = Math.round(
      number * Number.PRECISION
   ) / Number.PRECISION;
   

   if (number < 1)
   {
      number = 1/number;
      stream.write("-");
   }
   
   if (number == Number.POSITIVE_INFINITY)
   {
      stream.write("a");
      return stream;
   }
   
   // by definition
   if (number < Number.BASE)
   {
      stream.write(String(Math.floor(number)));
      return stream;
   }
      
   // use this to stop infinite
   // recursion. so by definition
   // open branch with two closed leaves
   // (2^0 + 0) == 1
   if (Math.floor(number) == Number.BASE) {
      stream.write("x00"); // leaf, leaf
      return stream;
   }
   
      
   // get the highest integral
   // exponent for base 2
   var exponent = getExponent(number);
         
   // get the remainder
   var remainder = getRemainder(
      number,
      exponent
   );
   
   // Write a "1" for an open branch for
   // power (left branch) and 
   // remainder (right branch)
   stream.write("x");
   
   // encode the exponent and
   // remainder branches
   exponent.encode(stream);   // branch
   remainder.encode(stream);  // branch
            
   return stream;
   
   // get the highest integral
   // exponent of a number
   function getExponent(number) {
      var exponent =
         Math.floor(
            Math.log(number) /
            Math.log(Number.BASE)
           // number / 10
         );
         
      return exponent;
   }
         
   // get the remainder of a
   // number minus its power
   function getRemainder(
      number, exponent
   )
   {
      var power =
           Math.pow(Number.BASE, exponent);
           //exponent * 10;
           
      var remainder = number - power;
   
      return remainder;

   }
         
}

Number.decode = function(stream)
{
   // get the next branch,
   var bit = stream.read();
   
   if (bit == "0")
      return 0;
   
   if (bit != "x")
      return Number(bit);
      
   // bit equals "x"
   // open branch
   
   // get the exponent
   var exponent = Number.decode(
      stream
   );
   
   // get the remainder
   var remainder = Number.decode(
      stream
   );
   
   // calculate the power
   var power =
      // Math.pow(Number.BASE, exponent);
      10 * exponent;
      
   // calculate the number
   var number = power + remainder;
      
   return number;
}