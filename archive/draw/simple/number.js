Number.BASE = 3;

Number.prototype.encode = function() {
      
   // by definition
   if (this == 0)
      return "0";
      
   // use this to stop infinite
   // recursion. so by definitin
   // open branch with two closed leaves
   if (this == 1)
      return "100";
      
   // get the highest integral
   // exponent
   var exponent = getExponent(this);
         
   // get the remainder
   var remainder = getRemainder(
      this,
      exponent
   );
   
   // add the exponent and
   // remainder branches
   var encoded = "1" + // open branch
      exponent.encode() +  // leaf
      remainder.encode();  // leaf
            
   return encoded;
            
            
   // get the highest integral
   // exponent of a number
   function getExponent(number) {
      var exponent =
         Math.floor(
            Math.log(number) /
            Math.log(Number.BASE)
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
         Math.pow(
            Number.BASE,
            exponent
         );
           
      var remainder =
         number - power;
   
      return remainder;

   }
         
}

Number.decode = function(encoded, index)
{

   if (!index)
      index = {
         position: 0
      }
      
   // get the next branch,
   // and move to the next position
   var bit = encoded[index.position++];
   
   if (bit == "0")
      // closed branch
      return 0;
  
   // open branch
   
   // get the exponent
   var exponent = Number.decode(
      encoded, index
   );
   
   // get the remainder
   var remainder = Number.decode(
      encoded, index
   );
   
   // calculate the power
   var power =
      Math.pow(Number.BASE, exponent);
      
   // calculate the number
   var number = power + remainder;
      
   return number;
}