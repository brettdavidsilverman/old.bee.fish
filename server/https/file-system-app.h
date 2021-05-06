#ifndef BEE_FISH_SERVER__FILE_SYSTEM_APP_H
#define BEE_FISH_SERVER__FILE_SYSTEM_APP_H

#include <filesystem>
#include "config.h"
#include "session.h"
#include "request.h"
#include "response.h"
#include "authentication.h"
#include "app.h"

using namespace std;
using namespace std::filesystem;

namespace bee::fish::https {

   class FileSystemApp : public App {
   
   public:
      inline static map<string, string>
         _contentTypes{
            {".txt",  "text/plain; charset=UTF-8"},
            {".html", "text/html; charset=UTF-8"},
            {".js",   "text/javascript; charset=UTF-8"}
         };
         
   public:
      FileSystemApp(
         Session* session
      ) : App(session)
      {
   
         string status = "200";
         
         Request* request = session->request();
         const BString& requestPath = request->path();
         
         Authentication auth(
            session
         );

         path filePath;
         
         // Get the file path from the request path
         try
         {
            filePath = getFilePath(requestPath);
           
            // Make sure file path is under
            // file system root
            if (!pathHasPrefix(filePath, fileSystemPath()))
               throw runtime_error("Invalid path accessing beyond root.");
    
         }
         catch (filesystem_error& err)
         {
            // Default error of not found
            status = "404";
         }
               
         // Redirect to remove trailing slashes
         // from directories
         if ( redirectDirectories(
                 requestPath,
                 filePath
              ) )
         {
            return;
         }
         
         std::ostringstream out;
      
         string origin;
   
         const Request::Headers& headers =
            request->headers();
      
         if (headers.contains("origin"))
            origin = headers["origin"];
         else if (headers.contains("host"))
            origin = headers["host"];
         else
            origin = HOST_NAME;
   

         out
            << "HTTP/1.1 "
            << status
            << " OK\r\n";
         
         string body;
         string contentType;
         size_t contentLength;
         
         
         if ( status == "200" &&
             !is_directory(filePath) )
         {
            if ( _contentTypes.count(
                    filePath.extension()
                 ) )
            {
               contentType = _contentTypes[
                  filePath.extension()
               ];
            }
            else
               status = "404";
         }
         
        // if ( status != "200" )
         {
            std::ostringstream bodyStream;
            write(bodyStream, status, auth, requestPath, filePath);

            body = bodyStream.str();
            
            contentType = "application/json; charset=UTF-8";
            contentLength = body.size();
            
            cerr << "********" << endl;
            cerr << body << endl;
            cerr << "********" << endl;
         }
         
         out
            << "content-type: "
               << contentType
               << "\r\n"
            << "content-length: "
               << std::to_string(contentLength)
               << "\r\n";
         
        // out
         //   << "connection: keep-alive\r\n";
      
         if (auth)
            out
               << "set-cookie: sessionId=" << auth.sessionId() << ";SameSite=None;Secure;HttpOnly\r\n";
         else
            out
               << "set-cookie: sessionId=" << "x" << ";SameSite=None;Secure;HttpOnly;max-age=-1\r\n";
         out
            << "Access-Control-Allow-Origin: "
               << origin
               << "\r\n"
            << "Access-Control-Allow-Credentials: "
               << "true\r\n"
            << "\r\n";
            
         if (body.size())
            out << body;
      
         _response = out.str();
   

      }
   

      bool pathHasPrefix(const path & child, const path & prefix)
      {
         auto pair = std::mismatch(child.begin(), child.end(), prefix.begin(), prefix.end());
         return pair.second == prefix.end();
      }
      
      path fileSystemPath() const
      {
         return path(FILE_SYSTEM_PATH);
      }
      
      void redirect(BString path)
      {
         stringstream out;
         out
            << "HTTP/1.1 " << "301" << " OK\r\n"
            << "connection: keep-alive\r\n"
            << "location: " << path << "\r\n"
            << "\r\n";
            
         _response = out.str();

      }
      
      bool redirectDirectories(const string& requestPath, const path& filePath)
      {
         if ( is_directory(filePath) &&
              requestPath != "/" )
         {
            if (requestPath[requestPath.size() - 1] == '/')
            {
               redirect(
                  requestPath.substr(
                     0, requestPath.size() - 1
                  )
               );
               return true;
            }
         }
         return false;
      }
      
      path getFilePath(const BString& requestPath) const
      {
         BString fullRequestPath =
            BString(FILE_SYSTEM_PATH) +
            requestPath;
               
         path filePath = canonical(
            path(fullRequestPath)
         );
            
         if (is_directory(filePath))
         {
            try
            {
               filePath =
                  canonical(
                     path(
                        fullRequestPath +
                        "/index.html"
                     )
                  );
            }
            catch(filesystem_error& err)
            {
            }
         }
         
         return filePath;
      }
      
      void write(ostream& out, const string& status, const Authentication& auth, const BString& requestPath, const path& filePath)
      {
         out << "{"
             << endl
             << "\t\"status\": "
             << status
             << "," << endl;
             
         out << auth << ", " << endl
                    << "\t\"requestPath\": "
                    << "\""
                       << requestPath
                    << "\"";
                    
         if (filePath != "")
         {
            out
               << "," << endl
               << "\t\"filePath\": "
               << filePath;
                    
            // extension
            if ( _contentTypes.count(
                  filePath.extension()
                ) )
            {
               string contentType =
                  _contentTypes[
                     filePath.extension()
                  ];
              
               out << "," << endl
                   << "\t\"contentType\":"
                   << "\"" << contentType << "\"";
            }
         }
         out << endl;
            
         out << "}";
      }
      
   };

   

}

#endif
