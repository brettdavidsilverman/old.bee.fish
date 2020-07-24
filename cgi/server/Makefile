CC = g++
CFLAGS = -std=c++2a -no-pie -fmax-errors=1 -g
PARSER = ../parser
DATABASE = ../database
INCLUDE = -I$(PARSER) -I$(DATABASE)

# Name of text file containing build number.
BUILD_NUMBER_FILE=build-number.txt
OBJECTS=session.o response.o main.o server.o md5.o base64.o $(PARSER)/parser.o $(DATABASE)/database.o $(DATABASE)/file.o

%.o: %.cpp
	$(CC) $< -c $(CFLAGS) $(INCLUDE)

server: $(OBJECTS) $(BUILD_NUMBER_FILE) Makefile
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o server $(OBJECTS) $(BUILD_NUMBER_LDFLAGS) -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' server

session.o: session.h session.cpp response.o

response.o: response.h request.h response.cpp basic-authorization.h base64.h md5.o token.h $(DATABASE)/database.h base64.o

server.o: server.h config.h server.cpp response.o md5.o

main.o: server.h version.h main.cpp server.o

md5.o: md5.h md5.cpp

base64.o: base64.h base64.cpp

parser:
	make -C $(PARSER) parser.o

database:
	make -C $(DATABASE) database.o

file:
	make -C $(DATABASE) file.o

clean:
	rm -f server
	rm -f server.o
	rm -f session.o
	rm -f response.o
	rm -f mtrace.txt
	rm -f session.txt
	rm -f main.o
	rm -f md5.o
	rm -f base64.o
	rm -f log
	rm -f trace.txt

# Include build number rules.
include build-number.mk