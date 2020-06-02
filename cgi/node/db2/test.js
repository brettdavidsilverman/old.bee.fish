const readline = require('readline');
require("../../power-encoding/string/string.js");
const DB2 = require("./db2/db2.js");

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
   var stream = line.encode();
   //db.walkPath(stream.bits);
   console.log(stream.bits);
});

read.on('close', function() {
/*
   const used = process.memoryUsage().heapUsed / 1024 / 1024;
   console.log(`Memory usage: ${Math.round(used * 100) / 100} MB`);
   const time = new Date() - start;
   console.log(`Time taken  : ${time}`);
   console.log(`Rows        : ${db.length / 2}`);
*/
});

//const fd = fs.openSync(__filename, "r+")