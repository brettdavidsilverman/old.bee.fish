#ifndef SSL_CONNECTION
#define SSL_CONNECTION

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "esp_crt_bundle.h"

#include <bee-fish.h>

void mbedtls_net_close_bds( mbedtls_net_context *ctx );

#undef ESP_LOGI

#define ESP_LOGI(tag, message, ...) { printf(message, ##__VA_ARGS__); printf("\r\n"); }

static const char *TAG = "ssl-connection";

namespace FeebeeCam {

   using namespace BeeFishBString;

   class SSLConnection {
   private:
      mbedtls_net_context _socket;

      mbedtls_ssl_context _ssl;
      mbedtls_entropy_context _entropy;
      mbedtls_ctr_drbg_context _ctr_drbg;
      mbedtls_x509_crt _cacert;
      mbedtls_ssl_config _conf;

      bool _socketCreated = false;
      bool _connected = false;
      bool _secureConnection = false;
   protected:
      const BString _host;
      const int _port;
      BString _path;
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
         
         int ret = 0;
         
         mbedtls_ssl_init(&_ssl);
         mbedtls_x509_crt_init(&_cacert);
         mbedtls_ctr_drbg_init(&_ctr_drbg);

         mbedtls_ssl_config_init(&_conf);

         mbedtls_entropy_init(&_entropy);
         if((ret = mbedtls_ctr_drbg_seed(&_ctr_drbg, mbedtls_entropy_func, &_entropy,
                                 NULL, 0)) != 0)
         {
            ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
            return false;
         }

         /* Hostname set here should match CN in server certificate */
         if((ret = mbedtls_ssl_set_hostname(&_ssl, _host.c_str())) != 0)
         {
            ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
            return false;
         }

         if((ret = mbedtls_ssl_config_defaults(&_conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
         {
            ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
            return false;
         }

         /* MBEDTLS_SSL_VERIFY_OPTIONAL is bad for security, in this example it will print
         a warning if CA verification fails but it will continue to connect.

         You should consider using MBEDTLS_SSL_VERIFY_REQUIRED in your own code.
         */
         mbedtls_ssl_conf_authmode(&_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
         mbedtls_ssl_conf_ca_chain(&_conf, &_cacert, NULL);
         mbedtls_ssl_conf_rng(&_conf, mbedtls_ctr_drbg_random, &_ctr_drbg);

         if ((ret = mbedtls_ssl_setup(&_ssl, &_conf)) != 0)
         {
            ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x\n\n", -ret);
            return false;
         }

         return true;

      }

      virtual bool open() {

         if (_connected)
            close();

         if (!initialize())
            return false;

         _connected = false;
         _secureConnection = false;

         int ret, flags, len;
         size_t written_bytes = 0;

         _socketCreated = false;
         
         mbedtls_net_init(&_socket);
         
         std::stringstream stream;
         stream << _port;
         std::string port = stream.str();
         std::string host = _host.str();

         clog << "Connecting to " << _host << ":" << _port << endl;

         if ( (ret = mbedtls_net_connect(
               &_socket,
               host.c_str(),
               port.c_str(),
               MBEDTLS_NET_PROTO_TCP)) != 0 )
         {
            ESP_LOGE(TAG, "mbedtls_net_connect returned -0x%x", -ret);
            return false;
         }

         _socketCreated = true;

         cerr << "Client Connected socket: " << _socket.fd << endl;

         _connected = true;

         clog << "Performing SSL handshake" << endl;

         mbedtls_ssl_set_bio(&_ssl, &_socket, mbedtls_net_send, mbedtls_net_recv, NULL);

         while ((ret = mbedtls_ssl_handshake(&_ssl)) != 0)
         {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
               ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
               return false;
            }
         }

         clog << "Securely connected" << endl;

//         ESP_LOGI(TAG, "Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&_ssl));

         _secureConnection = true;

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

         if (_socketCreated)
            mbedtls_net_close_bds( &_socket );

         _socketCreated = false;
         _connected = false;
         _secureConnection = false;

         shutdown();
      }
   
   protected:
      void shutdown() {

         cerr << "Shutting down SSLConnection" << endl;

         int ret = 0;

         mbedtls_x509_crt_free( &_cacert );
         mbedtls_ssl_free( &_ssl );
         mbedtls_ssl_config_free( &_conf );
         mbedtls_ctr_drbg_free( &_ctr_drbg );
         mbedtls_entropy_free( &_entropy );  

         cerr << "SSLConnection shutdown" << endl;

      }
   
   public:

      virtual bool connected() {
         return _connected;
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

         int ret = mbedtls_ssl_write(&_ssl, bytes, length);

         if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ)
            return 0;

         if (ret < 0) {
            ESP_LOGI(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
            return 0;
         }

            
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

         bzero(buffer, length);
         ret = mbedtls_ssl_read(&_ssl, buffer, length);

         if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            return 0;

         if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            ESP_LOGI(TAG, "MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY");
            cerr << "MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY" << endl;
            shutdown();
            return -1;
         }

         if(ret < 0)
         {
            ESP_LOGI(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
            cerr << "mbedtls_ssl_read failed" << endl;
            shutdown();
            return -1;
         }

         if(ret == 0)
         {
            //cerr << "mbedtls_ssl_read returned 0" << endl;
            return 0;
         }

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