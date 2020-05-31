const http = require('http');
const https = require('https');
const auth = require('http-auth');
const md5 = require('md5');
const fs = require('fs');
const url = require('url');
const path = require('path');
const events = require('events');
const process = require('process');
const KEY_FILE = "/etc/letsencrypt/live/bee.fish/privkey.pem";
const CERT_FILE = "/etc/letsencrypt/live/bee.fish/fullchain.pem";

// create a server object to handle
// redirects from http:80 to https:443
console.log("Http redirect to https listening.");

http.createServer(
   function (request, response) {
      response.statusCode = 301;
      response.setHeader(
         "Location",
         "https://bee.fish" +
            request.url
      );
      // end the response
      response.end("Redirecting..."); 
   }
).listen(80);


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

const digest = auth.digest(
   {
      realm: 'db2'
   },
   (username, callback, message) => { 
      // Custom authentication
      //console.log(message.headers["authorization"]);
      console.log(this);
      console.log(username);
      // Expecting md5(
      //   username:realm:password
      // ) in callback.
      if (username === "bee") {
         callback(md5("bee:db2:smart"));
      }
   }
);

const options = {
   key: fs.readFileSync(KEY_FILE),
   cert: fs.readFileSync(CERT_FILE)
};

var server = https.createServer(
   options, 
   digest.check((request, response) => {
      
      //result.end(request.user);
      
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
      // Is filename a directory?
      fs.promises.stat(filename)
      .then((st) => {
         if (st.isDirectory()) {
            // Set default page index.html
            filename += '/index.html';
         
            // Check that index file exists
            fs.promises.access(filename, fs.constants.R_OK)
            .then(() => {
               if (!request.url.endsWith("/")) {
                  console.log(request.url + "/");
                  response.statusCode = 301;
                  response.setHeader("Location", request.url + "/");
                  return response.end();
               }
            })
         }
         sendFile(filename);
      })
      .catch ((err) => {
         response.statusCode = 404;
         response.end(`File ${pathname} not found!`);
      })

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
         var stream = fs.createReadStream(filename, {highWaterMark: 1024});

         // Handle non-existent file
         stream.on('error', function(error) {
            response.statusCode = 500;
            response.end(`Error getting the file: ${error}.`);
         });

         // File exists, stream it to user
         response.statusCode = 200;
         response.setHeader('Content-type', file_ext[ext] || 'text/plain' );
                  
         if (ext == ".jpg")
            response.setHeader("Cache-control", "public, max-age=86400");
                 
         stream.pipe(response);
        
         return;
      }
   }
));


server.on('connection', (...event) => {
   console.log("connect");
});

server.listen(443);

console.log("HTTPS Server: 443");
