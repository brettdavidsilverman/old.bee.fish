function Stream(bits) {

   this.bits = bits;
   if (this.bits === undefined)
      this.bits = "";
      
   this.index = 0;
   this.plus = 0;

   this.peek = function() {
      if (this.index < this.bits.length)
         return this.bits[this.index];
      return "0";
   }
   
   this.read = function() {
       
      if (Number.isNaN(this.index) || this.bits === undefined)
         throw new Error("Before start of stream");
         
      if (this.index >= this.bits.length)
         throw new Error("End of stream");
         
      
      var bit = this.bits[this.index++];
      
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
             this.index,
             match.length
          ) === match) {
         this.index += match.length;
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