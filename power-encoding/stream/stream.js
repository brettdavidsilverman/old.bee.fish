function Stream(bits) {

   this.bits = bits;
   if (this.bits === undefined)
      this.bits = "";
      
   this.plus = 0;

   this.peek = function() {
      return this.bits[0];
   }
   
   this.read = function() {
      
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
   
   this.readMatch = function(match) {
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
   
   this.write = function(bits) {
   
      if (this.negative)
         bits = bits.split("").map(
            (bit) =>
               (bit == "1") ? "0" : "1"
         ).join("");
         
      this.bits += bits;
   }
   
   this.chain = function(downStream) {
      var ourRead = this.read
      this.read = function() {
         var bit = ourRead.call(this);
         downStream.write(bit);
      }
   }
   
   this.toString = function() {
      return this.bits;
   }
}

module.exports = Stream;