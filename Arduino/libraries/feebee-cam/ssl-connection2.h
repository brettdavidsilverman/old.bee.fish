#ifndef SSL_CONNECTION
#define SSL_CONNECTION

#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <bee-fish.h>
#include "bee.fish.cer.h"

namespace FeebeeCam {

   using namespace BeeFishBString;

   class SSLConnection {
   private:
      bool _secureConnection = false;
   protected:
      const BString _host;
      const int _port;
      BString _path;
      WiFiClientSecure _client;

   public:
      SSLConnection(const BString& host, int port) :
         _host(host),
         _port(port) 
      {
      }

      virtual ~SSLConnection() {
         close();
      }

      virtual bool initialize() {

//         _client.setInsecure();
         _client.setCACert(ca_cert);
         _client.connect(_host, _port);

         return true;

      }

      virtual bool open() {

         while (BeeFishWebServer::WebServer::processingClient())
               delay(10);

         if (connected())
            close();

         _secureConnection = false;

         std::stringstream stream;
         stream << _port;
         std::string port = stream.str();
         std::string host = _host.str();

         clog << "Connecting to " << _host << ":" << _port << endl;

         if (!initialize())
            return false;

//         cerr << "This is not secure... need to set certificate using _client.setCACert" << endl;

         _secureConnection = true;

//         clog << "Securely connected" << endl;
//         _secureConnection = true;

         return true;

      }

      BeeFishBString::BStream getStream() {
         BeeFishBString::BStream stream;
         stream.setOnBuffer(
            [this](const Data& buffer) {
               size_t bytesWritten = 0;
               while (bytesWritten < buffer.size()) {
                    bytesWritten += write(
                        buffer.data() + bytesWritten, 
                        buffer.size() - bytesWritten
                    );
               }
            }
         );
         return stream;
      }

      virtual void close() {

        if (connected())
            _client.stop();

         _secureConnection = false;
      }
   
    public:

        virtual bool connected() {
            return _client.connected();
        }

        virtual bool secureConnection() {
            return _secureConnection;
        }

        const BString& host() {
            return _host;
        }

        int port() {
            return _port;
        }

        virtual size_t write(const unsigned char* bytes, size_t length) {

            if (!secureConnection()) {
                throw std::runtime_error("Not securely connected");
            }

            int ret = _client.write(bytes, length);
                
            return ret;
            
        }

        virtual size_t write(const Data& data) {
            return write(data.data(), data.size());
        }

        virtual size_t write(const char* data) {
            return write((unsigned char*)data, strlen(data));
        }

      virtual size_t read(unsigned char* buffer, size_t length) {
         int ret;
         
         ret = _client.read(buffer, length);

         if (ret < 0)
            return 0;

         return ret;

      }

      virtual size_t read(Data& data) {
         return read(data.data(), data.size());
      }

      static bool test(const BString host, int port = 443, int count = 10) {

         bool result = true;

         std::stringstream stream;
         stream << "POST / HTTP/1.1\r\n"
                << "Host: " << host << "\r\n"
                << "Connection: keep-alive\r\n"
                << "User-Agent: feebee-cam/1.0 esp32\r\n"
                << "Content-Type: application/json\r\n"
                << "Content-Length: 22\r\n"
                << "\r\n"
                << "{\"method\":\"getStatus\"}";

         std::string request = stream.str();

         Data data = Data::create();

         printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

         SSLConnection* connection = new SSLConnection(host, port);
         
         if (!connection->open())
            return false;

         for (int i = 0; i < count && connection->secureConnection(); ++i) {

            cerr << "Sending post request" << endl;
            connection->write((const unsigned char*)request.c_str(), request.length());

            BeeFishWeb::WebResponse response;
            BeeFishBScript::BScriptParser parser(response);

            size_t length = 0;

            cerr << "Reading post response" << endl;
            while ( connection->secureConnection() &&
                     parser.result() == BeeFishMisc::nullopt ) 
            {
               if ((length = connection->read(data)) > 0) {
                  parser.read(data, length);
               }
            }
            
            if (parser.result() != true) {
               cerr << "Invalid parse response" << endl;
               result = false;
               break;
            }

            cerr << "Read response ok" << endl;
         }

         connection->close();

         delete connection;

         printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

         return result;

      }


      
   };
}

#endif