CC = g++
CFLAGS = -std=c++2a -no-pie -fmax-errors=1 -g
PARSER = ../../../parser
INCLUDE = -I$(PARSER)
# Name of text file containing build number.
BUILD_NUMBER_FILE=build-number.txt
OBJECTS=session.o response.o main.o server.o md5.o parser.o

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -c

server-asio: $(OBJECTS) $(BUILD_NUMBER_FILE) Makefile
	@echo server-asio
	@$(CC) $(CFLAGS) -O -o server-asio $(OBJECTS) $(BUILD_NUMBER_LDFLAGS) -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' server-asio

session.o: session.h session.cpp request.h response.h

response.o: response.h response.cpp

server.o: server.h config.h server.cpp

main.o: server.h version.h main.cpp

md5.o: md5.h md5.cpp

parser.o: $(PARSER)/parser.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(PARSER)/parser.cpp -c

clean:
	rm -f server-asio
	rm -f server.o
	rm -f session.o
	rm -f response.o
	rm -f mtrace.txt
	rm -f main.o
	rm -f md5.o
	rm -f parser.o
	rm -f log

# Include build number rules.
include build-number.mk