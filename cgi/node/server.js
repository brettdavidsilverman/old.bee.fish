// Load up modules
const http = require('http');
const https = require('https');
const auth = require('http-auth');
const md5 = require('md5');
const fs = require('fs');
const url = require('url');
const path = require('path');
const events = require('events');
const process = require('process');
const file = require('node-static');

// The host name for the server
const host = "bee.fish";
const base = "/home/bee/bee.fish";

var fileServer =
   new file.Server(
      base,
      {
         cache: false
      }
   );

// Set the secure certificates
// We use lets encrypt
// https://letsencrypt.org/
const KEY_FILE = "/etc/letsencrypt/live/" + host + "/privkey.pem";
const CERT_FILE = "/etc/letsencrypt/live/" + host + "/fullchain.pem";

// Setup the secure https server
setupSecure();

// Setup the redirect server
setupRedirectToSecure();


function setupRedirectToSecure() {
   // create a server object to handle
   // redirects from http:80 to https:443
   console.log("Http redirect to https listening.");

   var server = http.createServer(
      function (request, response) {
         response.statusCode = 301;
         response.setHeader(
            "Location",
            "https://" + host +
               request.url
         );
         // end the response
         response.end("Redirecting..."); 
      }
   );
   
   server.listen(80);
   
   return server;
}

function setupSecure() {

   const digest = auth.digest(
   {
      realm: 'db2'
   },
   (username, callback, request) => { 
      // Custom authentication
      console.log(username);
      // Expecting md5(
      //   username:realm:password
      // ) in callback.
      if (request.url.endsWith("logout"))
         callback(md5("wrong:db2:password"));
      else if (username === "bee") {
         callback(md5("bee:db2:smart"));
      }

   });

   const options = {
      key: fs.readFileSync(KEY_FILE),
      cert: fs.readFileSync(CERT_FILE),
      cache: false
   };

   var server = https.createServer(
      options,
      getPage
      //digest.check(getPage)
   );
   
   server.on('connection', (...event) => {
      console.log("connect");
   });

   server.listen(443);

   console.log("HTTPS Server: 443");
   
   return server;
   
   function getPage(request, response) {
   
      console.log(`${request.method} ${request.url}`);

      return fileServer.serve(request, response);
   }
    
}


