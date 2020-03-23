/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>

using namespace std;
using namespace boost::asio;

int pagesize;

int create_listener_socket(int port);
void handle_request(int client_socket);

int main(int argc, char *argv[])
{

   pagesize = getpagesize();
     
   int listener_socket, port;
   struct sockaddr_in client_address;
   socklen_t client_length;
   int thread_count;
     
   if (argc > 1)
      port = atoi(argv[1]);
   else
      port = 80;
      
   if (argc > 2)
      thread_count = atoi(argv[2]);
   else
      thread_count = 10;
        
   listener_socket =
      create_listener_socket(port);
 
   listen(listener_socket,5);
     
   client_length = sizeof(client_address);
   
   thread_pool threads(thread_count); 
  
   cout << "Listening on port "
        << to_string(port) << endl;
          
   for (;;) {
        
      try {
         int client_socket =
            accept(
               listener_socket, 
               (struct sockaddr *) &client_address, 
               &client_length
            );
                 
         if (client_socket < 0) {
            perror("Error accepting request");
           
            cerr << "Recreating listener socket." << endl;
            close(listener_socket);
            sleep(500);
            listener_socket =
               create_listener_socket(port);
            continue;
         }
         
         boost::asio::post(
            threads,
            [client_socket]() {
               try {
                  handle_request(
                     client_socket
                  );
               } catch (char const* error) {
                  if (error)
                     cout << error << endl;
               }
               close(client_socket);
            }
         );
      }
      catch (char const* error) {
         if (error)
            cerr << string(error) << endl;
         else
            cerr << "Unknown error." << endl;
      }
           
   }
     
   // This line is never reached
   close(listener_socket);

   return 0; 
}

int create_listener_socket(int port) {

  struct sockaddr_in server_address;
  
  // Create the socket.
  int listener_socket =
     socket(AF_INET, SOCK_STREAM, 0);
     
  if (listener_socket < 0)  {
     perror("Error opening socket");
     throw "Error opening socket.";
  }
 
  memset((char *) &server_address, 0, sizeof(server_address));
     
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
     
  // Set sockets address to being reusable.
  // This prevents "address already in use"
  // error.
  int yes = 1;
  setsockopt(
     listener_socket,
     SOL_SOCKET,
     SO_REUSEADDR,
     &yes, sizeof(int)
  );
    
  // Bind the socket to the addresss.
  if (bind(
         listener_socket,
         (struct sockaddr *) &server_address,
         sizeof(server_address)
      ) < 0) 
  {
     perror("Error on binding");
     throw "Error on binding listener socket.";
  }
  
  // Return the newly created socket.
  return listener_socket;

}

void handle_request(int client_socket) {
   char buffer[pagesize];
   
   int n =
      read(
         client_socket,
         buffer,
         pagesize
      );
  
   if (n < 0) {
      perror("Error reading from socket");
      return;
   }
  
   if (n == pagesize) {
      cerr << "Request too large." << endl;
      return;
   }
   
   string method;
   string path = "/";
   try {
      method = strtok(buffer, " "); 
      char* token = strtok(NULL, " ");
      if (token)
         path = string(token);
   }
   catch (...) {
      cerr << "Invalid request." << endl;
      return;
   }
              
   cerr << "Request" << "\t"
        << method << "\t"
        << path << endl;

   // This should redirect cout to 
   // the client socket.
   stringstream stream;
   stream
      << "HTTP/1.1 301 OK\r\n"
      << "location: https://bee.fish"
      << path << "\r\n"
      << "\r\n\r\n";
   string str = stream.str();
   write(client_socket, str.c_str(), str.length());

}
