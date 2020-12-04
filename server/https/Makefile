CC = g++
PARSER = ../parser
DATABASE = ../database
ID = ../id
INCLUDE = -I$(PARSER) -I$(DATABASE) -I$(ID)
CFLAGS = -std=c++2a $(INCLUDE) -Wall -fmax-errors=1

OBJECTS=session.o response.o main.o server.o md5.o wstring.o
DEPS=server.h session.h request.h response.h token.h md5.h wstring.h base64.h basic-authorization.h version.h Makefile config.h ../database/database

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

https: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o https $(OBJECTS) -lssl -lcrypto -lboost_system -lboost_thread -lpthread -latomic
#	sudo setcap 'cap_net_bind_service=+ep' https

install:
	./start

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	https

clean:
	rm -f session.log
	rm -f https
	rm -f *.o
	rm -f mtrace.txt