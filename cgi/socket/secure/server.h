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

#define STOP -99

class Server {
private:

// I use lets encrypt
// https://letsencrypt.org/
// These files are stored in
// /etc/letsencrypt...

   static const char cert_pem[];
   static const char key_pem[];
   static const size_t pagesize;
   
   
public:
   Server(int port = 443, const std::string& path = ".", int threadCount = 10);
   ~Server();
   
   int ssl_write(SSL* ssl, const std::string& text);
   int ssl_write_file(SSL* ssl, const std::string& path);

   
   void wait();
   void stop();
   bool stopped = false;
   const std::string& root_path;
   
private:
   SSL_CTX *ssl_context;
   int listener_socket;
   int _port;
   static boost::thread* main_thread;
   boost::asio::thread_pool* thread_pool;
   
   void init_openssl();
   void create_context();
   void configure_context();
   void create_listener_socket();
   bool check_for_input();
   void ssl_output(int client_socket);

   void cleanup_openssl();
   
   static void loop(Server* server);
   
};