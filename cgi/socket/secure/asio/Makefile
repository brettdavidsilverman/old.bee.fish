CC = g++
CFLAGS = -std=c++2a -fmax-errors=1 -g
INCLUDE = -I/home/bee/bee.fish/cgi/parser

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -c

server-asio: server.o session.o response.o main.o md5.o
	$(CC) $(CFLAGS) -O -o server-asio session.o response.o server.o main.o md5.o -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' server-asio

session.o: session.h session.cpp request.h response.h

response.o: response.h response.cpp

server.o: server.h config.h server.cpp

main.o: server.h version.h main.cpp

md5.o: md5.h md5.cpp

clean:
	rm -f server-asio
	rm -f server.o
	rm -f session.o
	rm -f response.o
	rm -f mtrace.txt
	rm -f main.o
	rm -f md5.o
