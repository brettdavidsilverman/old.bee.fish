CC = g++
PARSER = ./parser
DATABASE = ./database
ID = ./id
INCLUDE = -I$(PARSER) -I$(DATABASE) -I$(ID)
CFLAGS = -std=c++2a $(INCLUDE) -Wall -no-pie -fmax-errors=1

# Name of text file containing build number.
BUILD_NUMBER_FILE=build-number.txt
OBJECTS=session.o response.o main.o server.o md5.o base64.o
DEPS=server.h session.h request.h response.h token.h basic-authorization.h parser/parser database/database id/id power-encoding/power-encoding Makefile config.h

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

server: $(OBJECTS) $(DEPS) $(BUILD_NUMBER_FILE) Makefile
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o server $(OBJECTS) $(BUILD_NUMBER_LDFLAGS) -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' server

database/database:
	cd database && $(MAKE) $(DEBUG)

id/id:
	cd id && $(MAKE) $(DEBUG)
	
parser/parser:
	cd parser && $(MAKE) $(DEBUG)

power-encoding/power-encoding:
	cd power-encoding && $(MAKE) $(DEBUG)
	
debug: DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	server

clean:
	rm -f server
	rm -f *.o
	rm -f mtrace.txt
	rm -f session.txt
	rm -f session.log
	rm -f log
	make -C database clean
	make -C parser clean
	make -C id clean
	make -C power-encoding clean

# Include build number rules.
include build-number.mk
