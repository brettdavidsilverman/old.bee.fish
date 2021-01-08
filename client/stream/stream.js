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
   
   read()
   {
      
      var bit = this.bits[0];
      
      this.bits = this.bits.substr(1);
      
      if (bit === "1")
         this.count++;
      else
         this.count--;
      
      return bit;
   }

   write(bits)
   {
      this.bits += bits;
   }
   
   chain(downStream)
   {
      var ourRead = this.read
      this.read = function() {
         var bit = ourRead.call(this);
         downStream.write(bit);
      }
   }
   
   toString()
   {
      return this.bits;
   }
   
   get base64()
   {
      var bits = this.bits;
      
      bits = bits.padEnd(
         bits.length + bits.length % 8,
         "0"
      );
      
      var string = "";
      
      for (var i = 0; i < bits.length; i += 8)
      {
         var chunk = bits.substr(i, 8);
         var number = parseInt(chunk, 2);
         var c = String.fromCharCode(number);
         string += c;
      }
      
      return btoa(string);
   }
   
   static fromBase64(base64)
   {
   
      var string = atob(base64);
      var bits = ""
      for (var i = 0; i < string.length; ++i)
      {
         var number = string.charCodeAt(i);
         var chunk = number.toString(2);
         chunk = chunk.padStart(8, "0");
         bits += chunk;
      }
     
      return new Stream(bits);
   }
   
   
   
}
