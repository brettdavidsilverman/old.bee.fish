/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string>

int pagesize;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     pagesize = getpagesize();
     int sockfd, portno;
     socklen_t clilen;
     char* buffer = (char*)malloc(pagesize);
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     struct timeval _time;
     
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
      int yes = 1;
     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
     // Ignore SIGCHLD to avoid zombie threads
     signal(SIGCHLD, SIG_IGN);
     
     for (;;) {
        int newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
                 
        if (newsockfd < 0) 
           error("ERROR on accept");
           
        if (fork() == 0) {
           memset(buffer, 0, pagesize);
           n = read(newsockfd,buffer,pagesize - 1);
           if (n < 0) error("ERROR reading from socket");
           if (n == pagesize - 1) {
              printf("Uploads not permitted\r\n");
              close(newsockfd);
              continue;
           }
           std::string method = strtok(buffer, " "); 
           std::string path = "/";
           char* token = strtok(NULL, " ");
           if (token)
              path = std::string(token);
              
           fprintf(stderr, "http request(80) %s\r\n", path.c_str());
          // printf("***Response***\r\n");
           dup2(newsockfd, STDOUT_FILENO);
           printf("HTTP/1.1 301 OK\r\n");
           printf((std::string("location: https://bee.fish") + path + std::string("\r\n")).c_str());
           printf("\r\n\r\n");
           close(newsockfd);
           /*
           printf("Access-Control-Allow-Origin: http://bee.fish\r\n");
           printf("\r\n");
           gettimeofday(&_time, NULL);
           printf("%ld:%ld\r\n", _time.tv_sec, _time.tv_usec);
           printf("%s\r\n", buffer);
           */
           exit(0);
        }
        else
           close(newsockfd);
     }
     close(sockfd);
     free(buffer);
     return 0; 
}
