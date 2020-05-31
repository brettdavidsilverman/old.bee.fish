//const mmap = require("mmap-io");
const fs = require('fs');

fs.promises.readdir(".")
.then(files => {
   console.log(files.join("\n"));
})
.catch(err => {
   console.log(err);
});