if (typeof UInt16 === "undefined")
   UInt16 = require("./uint16.js");

UInt16.Cache = new Array(UInt16.MAX + 1);

for (i = 0; i <= UInt16.MAX; ++i) {
   var x = new UInt16(i);
   var stream = x.encode();
   UInt16.Cache[i] = stream.bits;
}
