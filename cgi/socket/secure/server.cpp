#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include "server.h"
const char Server::cert_pem[] = "/etc/letsencrypt/live/bee.fish/fullchain.pem";
const char Server::key_pem[]  = "/etc/letsencrypt/live/bee.fish/privkey.pem";
const size_t Server::pagesize = getpagesize();
boost::thread* Server::main_thread;


int Server::ssl_write(SSL* ssl, const std::string& text) {
   return SSL_write(ssl, text.c_str(), text.length());
}

int Server::ssl_write_file(SSL* ssl, const std::string& path) {
   //std::cout << path << std::flush << std::endl;

   std::ifstream file(path.c_str(), std::ifstream::in);
   if (!file.good()) {
      perror("Couldn't open file");
      return -1;
   }
   //std::cout << "here" << std::flush << std::endl;
   char buffer[pagesize];
   
   while (!file.eof()) {
     // memset(buffer, '\0', sizeof(buffer));
      
      file.read(buffer, sizeof(buffer));
      
      int read = file.gcount();
      
      SSL_write(
         ssl,
         buffer,
         read
      );
      
   }
   
   file.close();
   
   return 0;
}

void Server::create_listener_socket()
{
    struct sockaddr_in addr;
    int yes = 1;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket < 0) {
       perror("Unable to create socket");
	      throw "Unable to create listener socket.";
    }
    
    setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    
    if (bind(listener_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      	perror("Unable to bind");
	      throw "Unable to bind listener socket.";
    }

    if (listen(listener_socket, 1) < 0) {
      	perror("Unable to listen");
      	throw "Unable to listen.";
    }

}

void Server::init_openssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void Server::cleanup_openssl()
{
    EVP_cleanup();
}

void Server::create_context()
{
    const SSL_METHOD *method;

    method = SSLv23_server_method();

    ssl_context = SSL_CTX_new(method);
    
    if (!ssl_context) {
	      perror("Unable to create SSL context");
	      ERR_print_errors_fp(stdout);
       throw "Could not create SSL context";
    }

}

void Server::configure_context()
{
    SSL_CTX_set_ecdh_auto(ssl_context, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ssl_context, cert_pem, SSL_FILETYPE_PEM) <= 0) {
       ERR_print_errors_fp(stdout);
	      throw "Could not access certificate file.";
    }

    if (SSL_CTX_use_PrivateKey_file(ssl_context, key_pem, SSL_FILETYPE_PEM) <= 0 ) {
       ERR_print_errors_fp(stdout);
	      throw "Could not access private key file";
    }
}

void Server::ssl_output(int client_socket) {

   if (stopped)
      return;
      
   SSL *ssl = SSL_new(ssl_context);
   SSL_set_fd(ssl, client_socket);
     
   if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stdout);
   }
   else {
      int size = pagesize*4;
      char buffer[size];
      memset(buffer, 0, size);
      int ret;
      if (SSL_read(ssl, buffer, size) <=0) {
         ERR_print_errors_fp(stdout);
         SSL_shutdown(ssl);
         SSL_free(ssl);
         close(client_socket);
         throw "Error reading";
         return;
      }
      
      std::cout << buffer << std::endl;
      
      
      std::string method = strtok(buffer, " "); 
      std::string path = "";
      
      char* pathToken = strtok(NULL, " ");
      if (pathToken != NULL)
         path = std::string(pathToken);
         
      std::string content_location;
      std::string cache_control;
      
      if (boost::starts_with(path, "/"))
         path = path.substr(1);
         
      content_location = path;
      path = root_path + path;
      
      struct stat file_stat;
      
      int res = stat(path.c_str(), &file_stat);
      int response;
      bool redirect = false;
      bool is_directory = false;
      if (res == 0) {
         if (S_ISDIR(file_stat.st_mode)) {
            is_directory = true;
            if (boost::ends_with(path, "/") == false) {
               content_location += "/";
               path += "/";
               redirect = true;
            }
            path += "index.html";
   
         }
      }
    
      
      std::string content_type = "text/plain";
      if (boost::ends_with(path, ".cpp") ||
          boost::ends_with(path, ".h"))
         content_type = "text/plain";
         
      else if (boost::ends_with(path, ".css")) {
         content_type = "text/css";
      }
      else if (boost::ends_with(path, ".jpg")) {
         content_type = "image/jpeg";
         cache_control = "public, max-age=86400";
      }
      else if (boost::ends_with(path, ".txt")) {
         content_type = "text/plain";
      }
      else if (boost::ends_with(path, ".html")) {
         content_type = "text/html";
      }
      else if (boost::ends_with(path, ".js")) {
         content_type = "text/javascript";
      }
      else if (boost::ends_with(path, "/stop")) {
         path = "stop";
         content_type = "text/plain";
      }
      else {
         content_type = "text/plain";
      }
      
      std::cout << "Response: " << path << std::endl;
 
      std::string content_length;
      
      res = stat(path.c_str(), &file_stat);
      if (res == 0) {
         if (is_directory && redirect)
            response = 301;
         else {
            response = 200;
            size_t length = file_stat.st_size;
            content_length =
               std::to_string(length);
         }
      }
      else
         response = 404;
         
      std::stringstream headers;
      
      headers
      << "HTTP/1.1 "
         << std::to_string(response) << " OK\r\n"
      << "keep-alive: timeout=5, max=5\r\n"
      << "access-control-allow-origin: https://bee.fish\r\n";
      if (response == 301) {
         headers
            << "location: https://bee.fish/"
            << content_location << "\r\n";
      }
      else if (response == 200) {
         headers
            << "content-type: "
            << content_type << "\r\n"
            << "content-length: " 
            << content_length << "\r\n";
         if (cache_control.length() > 0)
            headers
               << "cache-control: "
               << cache_control << "\r\n";
      }
      
      headers << "\r\n";
      
      std::cout << headers.str() << std::endl;
      
      ssl_write(ssl, headers.str());

      if (response == 200)
         ssl_write_file(ssl, path);
         
      SSL_shutdown(ssl);
      SSL_free(ssl);
      close(client_socket);
      
      
      //if (path == "stop")
      //   stop();
      
   }
   
}


Server::Server(
   int port,
   const std::string& path,
   int threadCount) :
      root_path(path),
      _port(port)
{
   
   std::cout << "Starting server..." << std::endl;
  
   std::cout << "Initializing ssl..." << std::endl;

   init_openssl();
    
   create_context();
   
   configure_context();
   

   
   std::cout << "Creating listener socket.." << std::endl;;
   
   create_listener_socket();

   thread_pool = new boost::asio::thread_pool(threadCount); 
  
   Server::main_thread =
      new boost::thread(
         [this]() {
            Server::loop(this);
         }
      );
     
   std::cout << "https server started on port "
             << port 
             << " serving files from "
             << root_path
             << " with "
             << threadCount 
             << " threads."
             << std::endl;
             
   
}

void Server::stop() {
   std::cout << "Stopping..." << std::endl;
   stopped = true;
   close(listener_socket);
}

void Server::loop(Server* server) {

   // Handle connections 
   while(1) {

      struct sockaddr_in addr;
      uint len = sizeof(addr);
        
      int client_socket = accept(
         server->listener_socket,
         (struct sockaddr*)&addr,
         &len
      );
      
      if (server->stopped) {
         std::cout << "Exit main loop" << std::endl;
         return;
      }
      
      if (client_socket < 0) {
         perror("Unable to accept");
         close(server->listener_socket);
         server->create_listener_socket();
         continue;
      }
      
      boost::asio::dispatch(
         *(server->thread_pool),
         [server, client_socket]() {
            try {
               server->ssl_output(
                  client_socket
               );
            } catch (char* error) {
               std::cout << "*error:" << error << std::endl;
            }
         }
      );
   }

}

void Server::wait() {
   Server::main_thread->join();
}

Server::~Server() {
   std::cout << "Cleaning up..." << std::endl;
   
   if (stopped == false)
      stop();
      
   std::cout << "Main thread..." << std::endl;
   Server::main_thread->join();
   
   std::cout << "Thread pool..." << std::endl;
   thread_pool->join();

   delete thread_pool;
   delete main_thread;
  
   SSL_CTX_free(ssl_context);
   cleanup_openssl();
   std::cout << "Cleaned up." << std::endl;
}