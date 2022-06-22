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

#undef ESP_LOGI

#define ESP_LOGI(tag, message, ...) { printf(message, ##__VA_ARGS__); printf("\r\n"); }

static const char *TAG = "ssl-connection";

namespace FeebeeCam {

   using namespace BeeFishBString;

   class SSLConnection {
   private:
      mbedtls_ssl_context _ssl;
      mbedtls_net_context _socket;

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
         if (_connected)
            close();
      }

      virtual void open() {

         if (_connected)
            close();

         _connected = false;
         _secureConnection = false;

         int ret, flags, len;
         size_t written_bytes = 0;

         mbedtls_ssl_init(&_ssl);
         mbedtls_x509_crt_init(&_cacert);
         mbedtls_ctr_drbg_init(&_ctr_drbg);

         mbedtls_ssl_config_init(&_conf);

         mbedtls_entropy_init(&_entropy);
         if((ret = mbedtls_ctr_drbg_seed(&_ctr_drbg, mbedtls_entropy_func, &_entropy,
                                 NULL, 0)) != 0)
         {
            ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
            throw std::runtime_error("SSLConnection::mbedtls_ctr_drbg_seed");
         }

         /* Hostname set here should match CN in server certificate */
         if((ret = mbedtls_ssl_set_hostname(&_ssl, _host.c_str())) != 0)
         {
            ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
            throw std::runtime_error("SSLConnection::mbedtls_ssl_set_hostname");
         }

         if((ret = mbedtls_ssl_config_defaults(&_conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
         {
            ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
            throw std::runtime_error("SSLConnection::mbedtls_ssl_config_defaults");
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
            throw std::runtime_error("SSLConnection::mbedtls_ssl_setup");
         }

         _socketCreated = false;
         
         mbedtls_net_init(&_socket);
         
         _socketCreated = true;

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
            throw std::runtime_error("SSLConnection::mbedtls_net_connect");
         }

         clog << "Client Connected socket: " << _socket.fd << endl;

         _connected = true;

         clog << "Performing SSL handshake" << endl;

         mbedtls_ssl_set_bio(&_ssl, &_socket, mbedtls_net_send, mbedtls_net_recv, NULL);

         while ((ret = mbedtls_ssl_handshake(&_ssl)) != 0)
         {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
               ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
               throw std::runtime_error("SSLConnection::mbedtls_ssl_handshake");
            }
         }

         clog << "Securely connected" << endl;

         ESP_LOGI(TAG, "Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&_ssl));

         _secureConnection = true;

      }

      BeeFishBString::BStream getStream() {
         BeeFishBString::BStream stream;
         stream.setOnBuffer(
            [this](const Data& buffer) {
               cerr << "SSLConnection::BStream::onBuffer:: " << buffer.size() << endl;
               size_t bytesWritten = 0;
               while (bytesWritten < buffer.size()) {
                  bytesWritten += write(
                     buffer.data() + bytesWritten, 
                     buffer.size() - bytesWritten
                  );
               }
               cerr << "SSLConnection::BStream::~onBuffer:: " << bytesWritten << endl;
            }
         );
         return stream;
      }

      virtual void close() {
         
#warning "I believe this close hangs any other open server connection"

         if (_connected)  
            cerr << "Disconnecting from " << _host << endl;

         int ret = 0;
         do {
            
            ret = mbedtls_ssl_close_notify( &_ssl );
            
         } while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );  

//            mbedtls_ssl_session_reset(&_ssl);
         mbedtls_net_free( &_socket );

         mbedtls_x509_crt_free( &_cacert );
         mbedtls_ssl_free( &_ssl );
         mbedtls_ssl_config_free( &_conf );
         mbedtls_ctr_drbg_free( &_ctr_drbg );
         mbedtls_entropy_free( &_entropy );  

         _socketCreated = false;
         _connected = false;
         _secureConnection = false;

         cerr << "Connection closed" << endl;

      }

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
            cerr << "Not securely connected" << endl;
            throw std::runtime_error("Not securely connected");
         }

         int ret = mbedtls_ssl_write(&_ssl, bytes, length);

         cerr << "ssl-connection:write:ret: " << ret << endl;

         if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ)
            return 0;

         if (ret < 0) {
            ESP_LOGI(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
            throw std::runtime_error("SSLConnection::write::mbedtls_ssl_write");
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

         cerr << "ssl-connection:read:ret: " << ret << endl;
         
         if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            return 0;

         if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            ESP_LOGI(TAG, "MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY");
            close();
            return -1;
         }

         if(ret < 0)
         {
            ESP_LOGI(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
            close();
            throw std::runtime_error("SSLConnection::mbedtls_ssl_read");
         }

         if(ret == 0)
         {
            //close();
            ESP_LOGI(TAG, "returned 0");
            return -1;
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
         
         connection->open();

         for (int i = 0; i < count && connection->secureConnection(); ++i) {

            connection->write((const unsigned char*)request.c_str(), request.length());

            BeeFishWeb::WebResponse response;
            BeeFishBScript::BScriptParser parser(response);

            size_t length = 0;

            while ( connection->secureConnection() &&
                     parser.result() == BeeFishMisc::nullopt ) 
            {
               if ((length = connection->read(data)) > 0) {

                  parser.read(data, length);

               }
            }
            
            cerr << "Parser Result " << i << " {" << parser.result() << "}" << endl;
            if (parser.result() != true)
               result = false;
         }

         connection->close();

         delete connection;

         printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

         return result;

      }

      static void my_debug(void *ctx, int level, const char *file, int line,
                         const char *str)
      {
         const char *p, *basename;
         (void) ctx;

         /* Extract basename from file */
         for(p = basename = file; *p != '\0'; p++) {
            if(*p == '/' || *p == '\\') {
                  basename = p + 1;
            }
         }

         mbedtls_printf("%s:%04d: |%d| %s", basename, line, level, str);
      }

      
   };
}

#endif