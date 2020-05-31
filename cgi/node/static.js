var http = require('http');

var static = require('node-static');

var file = new static.Server();

http.createServer (function (request, response) {

file.serve (request, response);

response.writeHead(200, {'Content-Type': 'text/plain'});

response.end('Hello World!');

}).listen(80);

console.log("Server running");

