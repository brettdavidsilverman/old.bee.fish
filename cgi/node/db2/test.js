const readline = require('readline');
//require("../../../power-encoding/number/uint16/cache.js");
//require("../../../power-encoding/string/string.js");
const BitString = require("../../../power-encoding/bit-string/bit-string.js")
const DB2 = require("./db2.js");

String.prototype.encode = function() {
   var bitString =
      BitString.fromCharString(
         this.valueOf()
      );
   return bitString.bits;
}

var db = new DB2();
var read = readline.createInterface(
   {
      input: process.stdin,
      output: process.stdout,
      terminal: false
   }
);

var start = new Date();

read.on('line', function(line) {
   db.pointer = 0;
   var bits = line.encode();
   db.walkPath(bits + "0");
});

read.on('close', function() {

   const used = process.memoryUsage().heapUsed / 1024 / 1024;
   console.log(`Memory usage: ${Math.round(used * 100) / 100} MB`);
   const time = new Date() - start;
   console.log(`Time taken  : ${time}`);
   console.log(`Rows        : ${db.length / 2}`);
   
   db.walkPath("b".encode());
   var i = db.pointer;
   console.log({left: db[i],right: db[i+1]});
   var input = {
      count: 10,
      items: [],
      bits: "",
      index: db.pointer
   }
   
   db.next(input);
   
   console.log(input.items);
   
});

//const fd = fs.openSync(__filename, "r+")