#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H
#include <exception>
#include <optional>
#include "../database/database.h"
#include "../database/path.h"
#include "request.h"
#include "server.h"
#include "credentials.h"

using namespace bee::fish::database;
using namespace bee::fish::power_encoding;
using namespace bee::fish::https;

namespace bee::fish::https {

   class Authentication
   {
   protected:
      Database& _database;
   public:
      BString _ipAddress;
      BString _username;
      BString _secret;
      BString _thumbnail;
      
      bool _usernameExists = false;
      bool _authenticated = false;
      Path<PowerEncoding> _path;
      
   
   
      Authentication(Database& database) :
         _database(database),
         _path(database)
      {
      }
      
   public:
      Authentication(
         Session& session
      ) :
         Authentication(
           *( session.server()->database() )
         )
      {
      
         Request& request =
            *( session.request() );
         
         _ipAddress = session.ipAddress();
         
         if (request.hasBody())
         {
            _Object& body = request.body();
      
            if ( body.contains("username") )
            {
               _username = body["username"]->value();
               _usernameExists =
                  checkUsernameExists(_username);
            }
            
            if ( body.contains("secret") )
               _secret = body["secret"]->value();
  
            if ( body.contains("thumbnail") )
               _thumbnail = body["thumbnail"]->value();
               
            if ( body.contains("method") )
            {
               const BString& method =
                  body["method"]->value();
                    
               if ( method == "logon" )
               {
                  logon();
               }
               if ( method == "checkUsernameExists" )
               {
                  _thumbnail.clear();
               }
               else if ( method == "getThumbnail" )
               {
                  _thumbnail = getThumbnail(_username);
               }
               
            }
            

         }
         

      }
     
   public:
      virtual void logon()
      {
         if (!_ipAddress.size())
            throw runtime_error("Missing ip-address.");
            
         if (!_username.size())
            throw runtime_error("Missing username.");

         if (!_secret.size())
            throw runtime_error("Missing username.");
            
         if (_usernameExists)
         {
            // Check the secret to see if 
            // it matches the user.
            _authenticated =
               _path
                  ["Users"]
                  [_username]
                  ["Secrets"]
                  .contains(_secret);
         }
         else
         {
            if (!_thumbnail.size())
               throw runtime_error("New users must supply a thumbnail.");
               
            // new user
            _authenticated = true;
            // Save the secret
            _path
               ["Users"]
               [_username]
               ["Secrets"]
               [_secret];
                  
            _path
               ["Users"]
               [_username]
               ["Data"]
               ["Thumbnails"]
               .setData(_thumbnail);
            
         }
         
         _thumbnail.clear();
         
      }
      
      virtual bool checkUsernameExists(BString username)
      {
         // Check if the user exists.
         bool exists =
            _path
               ["Users"]
               .contains(username);
               
         return exists;

      }
      
      virtual BString getThumbnail(BString username)
      {
         if (!checkUsernameExists(username))
         {
            return "";
         }
         
         // Get the thumbnail
         Path thumbnails = _path
               ["Users"]
               [_username]
               ["Data"]
               ["Thumbnails"];
         
         BString thumbnail;
         thumbnails.getData(thumbnail);
         
         return thumbnail;
      }
      
   public:
   
      bee::fish::database::
         Path<PowerEncoding> userData()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         return
            _path
               ["Users"]
               [_username]
               ["Data"]
               ["Thumbnail"];
      }
      
      friend ostream&
      operator << (
         ostream& out,
         const Authentication& token
      )
      {
         token.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         out << "{" << endl
             << "\t\"authenticated\": "
                << (_authenticated ?
                   "true" :
                   "false")
             << "," << endl
             << "\t\"username\": \"";
             
         _username.writeEscaped(out);
          
         out << "\"";
         
         if (!_authenticated)
            out << "," << endl
                << "\t\"usernameExists\": "
                << (_usernameExists ?
                   "true" :
                   "false");
                
         if (_thumbnail.size())
            out << "," << endl
                << "\t\"thumbnail\": \""
                   << _thumbnail
                << "\"";
        
         out << endl
             << "}";
           
      }
      
      operator bool()
      {
         return _authenticated;
      }
   };
   
   

};

#endif