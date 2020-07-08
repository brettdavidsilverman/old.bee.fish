CC = g++
CFLAGS = -std=c++2a -fmax-errors=1
INCLUDE = -I/home/bee/bee.fish/cgi/parser

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -c

https_server_asio: server.o session.o https-session.o main.o
#	rm -f https-server-asio
	$(CC) $(CFLAGS) -O -o https-server-asio session.o https-session.o server.o main.o -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' https-server-asio

#reader_writer.o: reader_writer.h reader_writer.cpp

session.o: session.h session.cpp

https-session.o: https-session.h http-request.h https-session.cpp

server.o: server.h config.h server.cpp

main.o: server.h version.h main.cpp

clean:
	rm -f https-server-asio
	rm -f server.o
	rm -f session.o
	rm -f https-session.o
#	rm -f reader_writer.o
	rm -f main.o
