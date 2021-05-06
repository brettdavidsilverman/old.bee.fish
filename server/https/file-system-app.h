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
      struct MimeType
      {
         string contentType;
         string cacheControl;
      };
      
      inline static map<string, MimeType>
         _mimeTypes{
            {
               ".txt",
               {
                  "text/plain; charset=UTF-8",
                  "no-store, max-age=0"
               }
            },
            {
               ".html",
               {
                  "text/html; charset=UTF-8",
                  "no-store, max-age=0"
               }
            },
            {
               ".js",
               {
                  "text/javascript; charset=UTF-8",
                  "no-store, max-age=0",
               }
            },
            {
               ".css", 
               {
                  "text/css; charset=UTF-8",
                  "no-store, max-age=0"
               }
            },
            {
               ".jpg",
               {
                  "image/jpeg",
                  "public, max-age=604800, immutable"
               }
            }
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

         // Get the file path from the request path
         try
         {
            _filePath = getFilePath(requestPath);
           
            // Make sure file path is under
            // file system root
            if (!pathHasPrefix(_filePath, fileSystemPath()))
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
                 _filePath
              ) )
         {
            return;
         }
         
      
         string origin;
   
         const Request::Headers& headers =
            request->headers();
      
         if (headers.contains("origin"))
            origin = headers["origin"];
         else if (headers.contains("host"))
            origin = headers["host"];
         else
            origin = HOST_NAME;
   

         _serveFile = false;
        
         string contentType = "text/plain";
         string cacheControl = "no-store, max-age=0";
         
         if ( status == "200" &&
             !is_directory(_filePath) )
         {
            if ( _mimeTypes.count(
                    _filePath.extension()
                 ) )
            {
               MimeType mimeType = _mimeTypes[
                  _filePath.extension()
               ];
               contentType = mimeType.contentType;
               cacheControl = mimeType.cacheControl;
               
               _contentLength =
                  file_size(_filePath);
               _serveFile = true;
            }
            else
               status = "404";
         }
         
         if ( status != "200" )
         {
            std::ostringstream contentStream;
            write(contentStream, status, auth, requestPath, _filePath);

            _content = contentStream.str();
            
            contentType = "application/json; charset=UTF-8";
            _contentLength = _content.size();
            _serveFile = false;
            
            cerr << "********" << endl;
            cerr << _content << endl;
            cerr << "********" << endl;
         }
         
         std::ostringstream headerStream;
         headerStream
            << "HTTP/1.1 "
            << status
            << " OK\r\n";
         headerStream
            << "content-type: "
               << contentType
               << "\r\n"
            << "content-length: "
               << std::to_string(_contentLength)
               << "\r\n"
            << "cache-control: "
               << cacheControl
               << "\r\n";
               
         headerStream
            << "connection: keep-alive\r\n";
      
         if (auth)
            headerStream
               << "set-cookie: sessionId=" << auth.sessionId() << ";SameSite=None;Secure;HttpOnly\r\n";
         else
            headerStream
               << "set-cookie: sessionId=" << "x" << ";SameSite=None;Secure;HttpOnly;max-age=-1\r\n";
         headerStream
            << "Access-Control-Allow-Origin: "
               << origin
               << "\r\n"
            << "Access-Control-Allow-Credentials: "
               << "true\r\n"
            << "\r\n";
      
         _headers = headerStream.str();
         _headersLength = _headers.size();
         _bytesTransferred = 0;
        

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
         stringstream headerStream;
         headerStream
            << "HTTP/1.1 " << "301" << " OK\r\n"
            << "connection: keep-alive\r\n"
            << "location: " << path << "\r\n"
            << "\r\n";
            
         _headers = headerStream.str();

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
      
      void write(ostream& headerStream, const string& status, const Authentication& auth, const BString& requestPath, const path& filePath)
      {
         headerStream << "{"
             << endl
             << "\t\"status\": "
             << status
             << "," << endl;
             
         headerStream << auth << ", " << endl
                    << "\t\"requestPath\": "
                    << "\""
                       << requestPath
                    << "\"";
                    
         if (filePath != "")
         {
            headerStream
               << "," << endl
               << "\t\"filePath\": "
               << filePath;
                    
            // extension
            if ( _mimeTypes.count(
                  filePath.extension()
                ) )
            {
               MimeType mimeType =
                  _mimeTypes[
                     filePath.extension()
                  ];
              
               headerStream << "," << endl
                   << "\t\"contentType\":"
                   << "\"" << mimeType.contentType << "\"";
                   
               headerStream << "," << endl
                   << "\t\"cacheControl\":"
                   << "\"" << mimeType.cacheControl << "\"";

            }
         }
         headerStream << endl;
            
         headerStream << "}";
      }
      
   };

   

}

#endif
