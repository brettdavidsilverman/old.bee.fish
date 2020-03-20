function Stream(bits) {

   this.bits = bits;
   if (!this.bits)
      this.bits = "";
      
   this.index = 0;
   this.plus = 0;
   
   this.peek = function() {
      if (this.index < this.bits.length)
         return this.bits[this.index];
      return "0";
   }
   
   this.read = function() {
   
      var bit = this.bits[this.index++];
      
      if (bit == "1")
         this.plus++;
      else// if (this.plus > 0)
         this.plus--;
      
      return bit;
   }
   
   this.write = function(bits) {
      this.bits += bits;
   }
   
   this.chain = function(downStream) {
      var ourRead = this.read
      this.read = function() {
         var bit = ourRead.call(this);
         downStream.writeBit(bit);
      }
   }
}