CC = g++
CFLAGS = -std=c++17

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c

https-server-asio: server.o session.o https_session.o https_parser.o reader_writer.o
	$(CC) $(CFLAGS) -O -o https-server-asio session.o https_session.o server.o https_parser.o reader_writer.o -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' https-server-asio

https_parser.o: https_parser.h https_parser.cpp

https_session.o: https_session.h https_session.cpp

session.o: session.h session.cpp

server.o: server.h server.cpp

reader_writer.o: reader_writer.h reader_writer.cpp

clean:
	rm -f https-server-asio
	rm -f server.o
	rm -f session.o
	rm -f https_session.o
	rm -f https_parser.o
