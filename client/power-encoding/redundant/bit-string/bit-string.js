class BitString {
   
   bits = "";
   bitsPerCharacter = 16;
   
   constructor(input) {
      if (!input)
         input = {}
      
      var bitsPerCharacter =
         input.bitsPerCharacter;
      if (!bitsPerCharacter)
         bitsPerCharacter = 16;
         
      this.bitsPerCharacter =
         bitsPerCharacter;
         
      var bits = input.bits;
   
      if (!bits)
         bits = "";
   
      // right pad with "0" bits to 
      // character length
      while ((   bits.length %
              bitsPerCharacter) != 0)
         bits += "0";
      
      this.bits = bits;
   }
   
   toString() {
         
      var string = "";
      
      for ( var index = 0;
            index < this.bits.length;
            index += this.bitsPerCharacter )
      {
  
         // get character size
         // number of bits
         var charBits =
            this.bits.substr(
               index,
               this.bitsPerCharacter
            );
         
         while (charBits.length < 
                this.bitsPerCharacter)
            charBits = charBits + "0";
            
         // Convert bit string to number
         var charCode = parseInt(
            charBits, 2
         );
         
         // Convert number to character
         // and append to strint
         string += String.fromCharCode(
            charCode
         );
      }
     
      return string;
   }
 
   static fromCharString(string) {
      return BitString.fromString(
         {
            string,
            bitsPerCharacter: 8
         }
      );
   }

   static fromUnicodeString(string) {
      return BitString.fromString(
         {
            string,
            bitsPerCharacter: 16
         }
      );
   }

   static fromString(input) {

      var string = input.string;
   
      var bitsPerCharacter =
         input.bitsPerCharacter;
      
      if (bitsPerCharacter == undefined)
         bitsPerCharacter = 16;

      var bits = "";
      for ( var index = 0;
            index < string.length;
            index++ ) 
      {
   
         // Get the character code at
         // the index
         var charCode =
            string.charCodeAt(
               index
            );
         
         // convert the character code to
         // a base 2 string.
         var charBits =
            charCode.toString(2);
      
         // Add 0 padding to the start
         while (charBits.length < bitsPerCharacter)
            charBits = "0" + charBits;
 
         // Append the bits
         bits += charBits;
      
      }

      // Create a bit string object
      // from the bits
      var bitString = new BitString(
         {
            bitsPerCharacter,
            bits
         }
      );
   
      // return the bit string object
      return bitString;
   }
}

module.exports = BitString;


