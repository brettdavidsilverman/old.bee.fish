#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>
#include "../../debug.h"

class Server {
private:

// I use lets encrypt
// https://letsencrypt.org/
// These files are stored in
// /etc/letsencrypt...

   static const char cert_pem[];
   static const char key_pem[];
   
// the log files
   static const char error_log[];
   static const char request_log[];
   
   static const size_t pagesize;
   
   
public:
   Server(int port = 443, const std::string& path = ".", int threadCount = 10);
   ~Server();
   

   
   void wait();
   const std::string& root_path;
   
private:
   SSL_CTX *ssl_context;
   int listener_socket;
   int _port;
   static boost::thread* main_thread;
   boost::asio::thread_pool* thread_pool;
   
   void setup_log_files();
   void setup_log_file(const char* filename, int replace);
   void init_openssl();
   void create_context();
   void configure_context();
   void create_listener_socket();
   
   void handle_request(int client_socket, const std::string& ip_address);
   void read(SSL* ssl, std::string& method, std::string& path);
   int write(SSL*, const std::string& method, const std::string& path);

   void ssl_write(SSL* ssl, const std::string& text);
   void ssl_write_file(SSL* ssl, const std::string& path);

   std::string generate_directory_html(const std::string& path);
   static std::string base_name(std::string const & path);
   
   void cleanup_openssl();
   
   static void loop(Server* server);
   
};