CC = g++
CFLAGS = -std=c++17

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c

https_server_asio: server.o session.o https_session.o reader_writer.o main.o
	rm -f https_server_asio
	$(CC) $(CFLAGS) -O -o https_server_asio session.o https_session.o server.o reader_writer.o main.o -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' https_server_asio

reader_writer.o: reader_writer.h reader_writer.cpp

session.o: session.h session.cpp

https_session.o: https_session.h https_session.cpp

server.o: server.h config.h server.cpp

main.o: server.h version.h main.cpp

clean:
	rm -f https_server_asio
	rm -f server.o
	rm -f session.o
	rm -f https_session.o
	rm -f main.o
	rm -f reader_writer.o