class Stream
{
   constructor(bits)
   {

      this.bits = bits;
      if (this.bits === undefined)
         this.bits = "";
   
      this.negative = false;
   
      this.plus = 0;
   }
   
   peek()
   {
      var bit = this.bits[0];
      if (this.negative)
         bit = bit === "1" ? "0" : "1";
         
      return bit;
   }
   
   read() {
      
      var bit = this.bits[0];
      
      this.bits = this.bits.substr(1);
      
      if (this.negative)
         bit = (bit == "1") ? "0" : "1";
         
      if (bit == "1")
         this.plus++;
      else
         this.plus--;
      
      return bit;
   }
   
   readMatch(match) {
      if (this.bits.substr(
             0,
             match.length
          ) === match)
      {
         this.bits = this.bits.substr(
            match.length
         );
         return true;
      }
      else
         return false;
   }
   
   write(bits) {
   
      if (this.negative)
         bits = bits.split("").map(
            (bit) =>
               (bit == "1") ? "0" : "1"
         ).join("");
         
      this.bits += bits;
   }
   
   chain(downStream) {
      var ourRead = this.read
      this.read = function() {
         var bit = ourRead.call(this);
         downStream.write(bit);
      }
   }
   
   toString() {
      return this.bits;
   }
   
   negate()
   {
      this.negative = !this.negative;
   }
}

module.exports = Stream;