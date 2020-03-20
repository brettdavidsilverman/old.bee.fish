Number.BASE = 2;

Number.prototype.encode = function(stream) {
   
   if (!stream)
      stream = new Stream();

   // by definition
   if (this <= 9) {
      stream.write(String(this));
      return stream;
   }
      
   // use this to stop infinite
   // recursion. so by definition
   // open branch with two closed leaves
   // (2^0 + 0) == 1
   if (this == 10) {
      stream.write("x00"); // leaf, leaf
      return stream;
   }
   
      
   // get the highest integral
   // exponent for base 2
   var exponent = getExponent(this);
         
   // get the remainder
   var remainder = getRemainder(
      this,
      exponent
   );
   
   // Write a "1" for an open branch for
   // power (left branch) and 
   // remainder (right branch)
   stream.write("1");
   
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
          //  Math.log(number) /
          //  Math.log(Number.BASE)
            number / 10
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
       //  Math.pow(Number.BASE, exponent);
           exponent * 10;
           
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
   
   // bit equals "1"
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