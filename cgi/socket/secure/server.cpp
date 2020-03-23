#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <filesystem>
#include "server.h"
#include "../config.h"

const size_t Server::pagesize = getpagesize();
boost::thread* Server::main_thread;

std::string Server::base_name(std::string const & path)
{
  return path.substr(path.find_last_of("/") + 1);
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

void Server::handle_request(int client_socket) {
   
   SSL *ssl = SSL_new(ssl_context);
   SSL_set_fd(ssl, client_socket);
     
   if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stdout);
      return;
   }

         
   std::string method;
   std::string path;
   
   read(ssl, method, path);
   write(ssl, method, path);
   
   SSL_shutdown(ssl);
   SSL_free(ssl);

}

void Server::read(SSL* ssl, std::string& method, std::string& path) {
   
   bool first = true;
   char buffer[pagesize];
   int ret;
   while (1) {
   
      memset(buffer, 0, pagesize);
      ret = SSL_read(ssl, buffer, pagesize);
      if (ret < 0)
         break;
         
      std::string buff = buffer;

      if (first) {
         size_t first_space =
            buff.find_first_of(" ");
            
         method =
            buff.substr(0, first_space); 
         
         size_t second_space =
            buff.find_first_of(
               " ",
               first_space + 1
            );
         
         path =
            buff.substr(
               first_space + 1,
               second_space - first_space - 1
            );
         
         first = false;
      }
      
      if (ret < pagesize)
         break;
   }
   
   if (ret < 0) {
      ERR_print_errors_fp(stdout);
      SSL_shutdown(ssl);
      SSL_free(ssl);
      throw "Error reading from ssl socket";
   }
   
   std::cout
      << "Request\t"
      << method 
      << "\t" << path 
      << std::endl;

}

void Server::write(SSL* ssl, const std::string& method, const std::string& path) {

   std::string location;
   std::string cache_control;
   std::string file_path = path;
   
   if (boost::starts_with(file_path, "/"))
      file_path = file_path.substr(1);
         
   location = path;
   file_path = root_path + file_path;
      
   struct stat file_stat;
      
   int res = stat(file_path.c_str(), &file_stat);
   int response;
   bool redirect = false;
   bool is_directory = false;
   if (res == 0) {
      if (S_ISDIR(file_stat.st_mode)) {
         is_directory = true;
         if (boost::ends_with(file_path, "/") == false) {
            location += "/";
            file_path += "/";
            redirect = true;
         }
            /*
            std::string index = path + "index.html";
            res = stat(index.c_str(), &file_stat);
            if (res == 0) {
               is_directory = false;
               path = index;
            }
            */
      }
   }
    
      
   std::string content_type = "text/plain";
   if (boost::ends_with(file_path, ".cpp") ||
       boost::ends_with(file_path, ".h"))
      content_type = "text/plain";
         
   else if (boost::ends_with(file_path, ".css")) {
      content_type = "text/css";
   }
   else if (boost::ends_with(file_path, ".jpg")) {
      content_type = "image/jpeg";
      cache_control = "public, max-age=86400";
   }
   else if (boost::ends_with(file_path, ".txt")) {
      content_type = "text/plain";
   }
   else if (boost::ends_with(file_path, ".html")) {
      content_type = "text/html";
   }
   else if (boost::ends_with(file_path, ".js")) {
      content_type = "text/javascript";
   }
   else {
      content_type = "text/plain";
   }
      
   std::string content_length;
   std::string html;
      
   res = stat(file_path.c_str(), &file_stat);
   if (res == 0) {
      if (is_directory) {
         if (redirect)
            response = 301;
         else {
            response = 200;
            content_type = "text/html";
            html = generate_directory_html(file_path);
            content_length =
               std::to_string(html.length());
         }
      }
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
      << "access-control-allow-origin: https://bee.fish\r\n";
   if (response == 301) {
      headers
         << "location: https://bee.fish/"
         << location << "\r\n";
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
      
   std::cout << "Response:\t" << file_path << std::endl;
      
   ssl_write(ssl, headers.str());

   if (response == 200) {
      if (is_directory)
         ssl_write(ssl, html);
      else
         ssl_write_file(ssl, file_path);
   }
      
   
   
}

void Server::ssl_write(SSL* ssl, const std::string& text) {
   SSL_write(ssl, text.c_str(), text.length());
}

void Server::ssl_write_file(SSL* ssl, const std::string& path) {
  
   std::ifstream file(path.c_str(), std::ifstream::in);
   if (!file.good()) {
      perror("Couldn't open file");
      throw "Couldnt't open file";
   }
   
   char buffer[pagesize];
   
   while (!file.eof()) {
      
      file.read(buffer, sizeof(buffer));
      
      int read = file.gcount();
      
      SSL_write(
         ssl,
         buffer,
         read
      );
      
   }
   
   file.close();
   
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

void Server::loop(Server* server) {

   // Handle connections 
   while(1) {

      try {
         struct sockaddr_in addr;
         uint len = sizeof(addr);
        
         int client_socket = accept(
            server->listener_socket,
            (struct sockaddr*)&addr,
            &len
         );
      
         if (client_socket < 0) {
            perror("Unable to accept");
            close(server->listener_socket);
            server->create_listener_socket();
            continue;
         }
      
         boost::asio::post(
            *(server->thread_pool),
            [server, client_socket]() {
               try {
                  server->handle_request(
                     client_socket
                   );
               } catch (char const* error) {
                  if (error)
                     std::cerr << error << std::endl;
                  else
                     std::cerr << "Unknown error." << std::endl;
               }
               close(client_socket);
            }
         );
      }
      catch(char const* error) {
         if (error)
            std::cerr << "Error: " << error << std::endl;
         else
            std::cerr << "Unknown error" << std::endl;
      }
      catch (...) {
         std::cerr << "Unknown error" << std::endl;
      }
   }

}

void Server::wait() {
   Server::main_thread->join();
}

Server::~Server() {
   std::cout << "Cleaning up..." << std::endl;
   
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

std::string Server::generate_directory_html(const std::string& path) {

   std::stringstream stream;
   
   std::vector<std::string> directories;
   std::vector<std::string> files;
   
   stream
      <<   "<html>"
      <<      "<body>";
      
   for (const auto & entry :
        std::filesystem::directory_iterator(path)
       )
   {
      std::string path_name =
         base_name(entry.path().string());
      if (entry.is_directory())
         directories.push_back(path_name);
      else
         files.push_back(path_name);
   }
   
   stream << "<a href=\"..\">Parent</a><br/>\r\n";
   std::sort(files.begin(), files.end());
   std::sort(directories.begin(), directories.end());
            
   for (const auto & dir_name : directories) {
      stream
         << "<a href=\""
         << dir_name + "/"
         << "\">+"
         << dir_name
         << "</a>"
         << "<br/>"
         << "\r\n";
   }
            
   for (const auto & file_name : files) {
      stream
         << "<a href=\""
         << file_name
         << "\">"
         << file_name
         << "</a>"
         << "<br/>"
         << "\r\n";
                  
   }
   
   stream
      <<      "</body>"
      <<   "</html>";
      
   return stream.str();
   
}

