const https = require('https');
const fs = require('fs');
const url = require('url');
const path = require('path');
const events = require('events');
const process = require('process');
const KEY_FILE = "/etc/letsencrypt/live/bee.fish/privkey.pem";
const CERT_FILE = "/etc/letsencrypt/live/bee.fish/fullchain.pem";
 
// maps file extention to MIME typere
const file_ext = {
   '.ico': 'image/x-icon',
   '.html': 'text/html',
   '.js': 'text/javascript',
   '.json': 'application/json',
   '.css': 'text/css',
   '.png': 'image/png',
   '.jpg': 'image/jpeg',
   '.wav': 'audio/wav',
   '.mp3': 'audio/mpeg',
   '.svg': 'image/svg+xml',
   '.pdf': 'application/pdf',
   '.doc': 'application/msword'
};

const options = {
   key: fs.readFileSync(KEY_FILE),
   cert: fs.readFileSync(CERT_FILE)
};

var server = https.createServer(
   options, 
   function (request, response) {
      //response.setHeader('Access-Control-Allow-Credentials', true);
      console.log(`${request.method} ${request.url}`);

      // parse URL
      const parsedUrl = url.parse(request.url);
      // extract URL path
      var pathname = parsedUrl.pathname;
      // extract filrme name
      var filename = getFilename(pathname);
      
      console.log(filename);
      
      // if is a directory search for index file
      try {
         fs.accessSync(filename, fs.constants.R_OK);
         // Is filename a directory?
         if (fs.statSync(filename).isDirectory()) {
            // Set default page index.html
            filename += '/index.html';
            // Check that index file exists
            fs.accessSync(filename, fs.constants.R_OK);
         }
         sendFile(filename);
      }
      catch (err) {
         response.statusCode = 404;
         response.end(`File ${pathname} not found!`);
      }

      function getFilename(pathname) {
         var filename = pathname;
         if (filename.startsWith("/"))
            filename = filename.substr(1);
         
         if (filename.endsWith("/"))
            filename = filename.substr(0, pathname.length - 1);
         
         if (filename == "")
            filename = ".";
         
         if (filename.includes("..")) 
            throw new Error("Security check " + filename);
         
         filename = path.resolve(process.cwd() + "/" + filename);
      
         return filename;
      }
      
      function sendFile(filename) {
         
          // based on the URL path, extract the file extention. e.g. .js, .doc, ...
         const ext = path.parse(filename).ext;
         
         // read file from file system
         fs.readFile(
            filename,
            function(err, data){
               if (err) {
                  response.statusCode = 500;
                  response.end(`Error getting the file: ${err}.`);
               } else {
                  // if the file is found, set Content-type and send data
                  response.statusCode = 200;
                  response.setHeader('content-type', file_ext[ext] || 'text/plain' );
                  
                  if (ext == ".jpg")
                     response.setHeader("cache-control", "public, max-age=86400");
                     
                  response.end(data);
               }
            }
         );
      }
   }
);


server.on('connection', (...event) => {
   console.log("connect");
});

server.listen(443);

console.log("HTTPS Server: 443");

