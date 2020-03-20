String.prototype.encode = function(stream) {
   
   if (!stream)
      stream = new Stream();
   
   for (var i = 0; i < this.length ; i++) {
      stream.write("1");
      var charCode = this.charCodeAt(i);
      charCode.encode(stream);
   }
   stream.write("0");
   
   return stream;
}

String.decode = function(stream) {
 
   var string = "";
   
   while (stream.read() == "1") {
      var charCode =
         Number.decode(stream);
      string +=
         String.fromCharCode(charCode);
   }
   
   return string;
   
}