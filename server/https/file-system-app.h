#ifndef BEE_FISH_SERVER__FILE_SYSTEM_APP_H
#define BEE_FISH_SERVER__FILE_SYSTEM_APP_H

#include <filesystem>
#include <algorithm>
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
         
      inline static vector<BString>
         _privileged{
            "/feebee.jpg",
            "/client/logon/",
            "/client/logon/index.html",
            "/head.js",
            "/style.css",
            "/client/console/console.js",
            "/client/sha3/sha3.js",
            "/client/sha3/hash-file.js",
            "/body.js",
            "/client/style.css",
            "/client/logon/style.css",
            "/client/logon/index.html",
            "/client/logon/authentication.js"
         };
         
   public:
      FileSystemApp(
         Session* session
      ) : App(session)
      {
   
         _status = "200";
         
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
                 requestPath,
                 _filePath
              ) )
         {
            return;
         }
         
         if ( !auth &&
              !isPrivileged(requestPath) )
         {
            redirect("/client/logon/", false);
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
            
            write(contentStream, _status, auth, requestPath, _filePath);

            contentType = "application/json; charset=UTF-8";
            
            _serveFile = false;
            
         }
         
         _headers["content-type"] =
            contentType;
         
         _headers["cache-control"] =
            cacheControl;
            
         _headers["connection"] = 
            "keep-alive";
      
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
      
      void redirect(BString path, bool permanent)
      {
         stringstream headerStream;
 
         if (permanent)
            _status = "301";
         else
            _status = "307";
            
         _headers.clear();
         
         _headers["connection"] = "keep-alive";
         _headers["location"] = path;
  
         _content = "redirecting...";
         _serveFile = false;
      }
      
      bool redirectDirectories(const string& requestPath, const path& filePath)
      {
         if ( is_directory(filePath) &&
              requestPath != "/" )
         {
            if (requestPath[requestPath.size() - 1] != '/')
            {
               redirect(requestPath + "/", true);
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
      
      bool isPrivileged(const BString& path)
      {
         return
            ( std::find(
                _privileged.begin(),
                _privileged.end(),
                path )
             != _privileged.end() );
      }
      
   };

   

}

#endif
