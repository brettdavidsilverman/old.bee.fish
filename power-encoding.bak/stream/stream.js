class Stream
{
   constructor(bits)
   {

      this.bits = bits;
      if (this.bits === undefined)
         this.bits = "";
   
      this.count = 0;
   }
   
   copy()
   {
      var stream = new Stream(this.bits);

      return stream;
   }
   
   peek()
   {
      var bit = this.bits[0];
      return bit;
   }
   
   read() {
      
      var bit = this.bits[0];
      
      this.bits = this.bits.substr(1);
      
      if (bit == "1")
         this.count++;
      else
         this.count--;
      
      return bit;
   }

   write(bits) {
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
   

}
