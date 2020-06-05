if (typeof Stream === "undefined")
   Stream = require("../stream/stream.js");
if (typeof UInt16 === "undefined")
   UInt16 = require("../number/uint16/uint16.js");

if (typeof UInt16.Cache === "undefined") {
   require("../uint16/cache.js");
}

String.prototype.encode =
 function(stream = new Stream()) {
   
   for (var i = 0; i < this.length ; ++i) {
      stream.write("1");
      var charCode = this.charCodeAt(i);
      var bits = UInt16.Cache[charCode];
      stream.write(bits);
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
