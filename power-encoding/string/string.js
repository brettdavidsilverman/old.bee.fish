String.prototype.encode =
 function(stream = new Stream()) {
   
   for (var i = 0; i < this.length ; ++i) {
      stream.write("1");
      var charCode = this.charCodeAt(i);
      UInt16(charCode)
         .encode(stream);
   }
   
   stream.write("0");
   
   return stream;
}

String.decode = function(stream) {
 
   var string = "";
   
   while (stream.read() == "1") {
      var charCode =
         UInt16.decode(stream);
      string +=
         String.fromCharCode(
            Number(charCode)
         );
   }
   
   return string;
   
}