if (typeof Stream === "undefined")
   Stream = require("../stream/stream.js");
if (typeof UInt16 === "undefined")
   UInt16 = require("../number/uint16/uint16.js");

String.prototype.encode =
 function(stream = new Stream()) {
   
   for (var i = 0; i < this.length ; ++i) {
      stream.write("1");
      var charCode = this.charCodeAt(i);
      if (UInt16.Cache) {
         var bits = UInt16.Cache[charCode];
         stream.write(bits);
      }
      else {
         var uint16 = new UInt16(charCode);
         uint16.encode(stream);
      }
   }
   
   stream.write("0");
   
   return stream;
}

String.decode = function(stream) {
 
   var string = "";
   
   while (stream.read() === "1") {
      var charCode =
         UInt16.decode(stream);
      string +=
         String.fromCharCode(
            Number(charCode)
         );
   }
   
   return string;
   
}
