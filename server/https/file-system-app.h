#ifndef BEE_FISH_SERVER__FILE_SYSTEM_APP_H
#define BEE_FISH_SERVER__FILE_SYSTEM_APP_H

#include <filesystem>
#include <algorithm>
#include "../config.h"
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
                  "public, max-age=31536000, immutable"
               }
            }
         };
         
      
         
   public:
      FileSystemApp(
         Session* session,
         Headers& responseHeaders
      ) : App(session, responseHeaders)
      {
   
         _status = "200";
         
         Request* request = session->request();
         const BString& requestPath = request->path();
         

         // Get the file path from the request path
         try
         {
            _filePath = getFilePath(requestPath);
           
            // Make sure file path is under
            // file system root
            if (!pathIsChild(_filePath, fileSystemPath()))
               throw runtime_error("Invalid path accessing beyond root.");
    
         }
         catch (filesystem_error& err)
         {
            // Default error of not found
            _status = "404";
         }
               
         // Redirect to remove trailing slashes
         // from directories
         if ( redirectDirectories(
                 *request,
                 _filePath
              ) )
         {
            return;
         }
         
         _serveFile = false;
        
         string contentType = "text/plain";
         string cacheControl = "no-store, max-age=0";
         
         if ( _status == "200" &&
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
               _serveFile = true;
            }
            else
               _status = "404";
         }
         
         std::ostringstream contentStream;
         
         if ( _status != "200" )
         {
            
            write(contentStream, _status, requestPath, _filePath);

            contentType = "application/json; charset=UTF-8";
            
            _serveFile = false;
            
         }
         
         responseHeaders.replace(
            "content-type",
            contentType
         );
         
         responseHeaders.replace(
           "cache-control",
            cacheControl
         );
            
         responseHeaders.replace(
            "connection",
            "keep-alive"
         );
      
         _content = contentStream.str();

      }
      
      bool pathIsChild(const path & child, const path & prefix)
      {
         auto pair = std::mismatch(child.begin(), child.end(), prefix.begin(), prefix.end());
         return pair.second == prefix.end();
      }
      
      path fileSystemPath(string child = "") const
      {
         return path(FILE_SYSTEM_PATH + child);
      }
      

      bool redirectDirectories(const Request& request, const path& filePath)
      {
         if ( is_directory(filePath) &&
              request.path() != "/" )
         {
            const BString& path =
               request.path();
               
            if (path[path.size() - 1] != '/')
            {
               BString newPath =
                  path + "/" +
                  request.query();
                  
               redirect(newPath, true);
               
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
                        "index.html"
                     )
                  );
            }
            catch(filesystem_error& err)
            {
            }
         }
         
         return filePath;
      }
      
      void write(ostream& headerStream, const string& status, const BString& requestPath, const path& filePath)
      {
         headerStream << "{"
             << endl
             << "\t\"status\": "
             << status
             << "," << endl;
             
         Authentication::write(headerStream);
         
         headerStream << ", " << endl
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
      

      virtual BString name()
      {
         return "File System";
      }
   };

   

}

#endif
