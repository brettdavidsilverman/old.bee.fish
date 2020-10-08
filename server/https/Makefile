CC = g++
PARSER = ../parser
DATABASE = ../database
ID = ../id
INCLUDE = -I$(PARSER) -I$(DATABASE) -I$(ID)
CFLAGS = -std=c++2a $(INCLUDE) -Wall -no-pie -fmax-errors=1

OBJECTS=session.o response.o main.o server.o md5.o base64.o wstring.o
DEPS=server.h session.h request.h response.h token.h md5.h wstring.h basic-authorization.h Makefile config.h

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

https: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o https $(OBJECTS) -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' https

install:
	./start

debug:
	DEBUG = debug
	CFLAGS += -g -D DEBUG
	https

clean:
	rm -f session.log
	rm -f bee.fish.data
	rm -f https
	rm -f *.o
	rm -f mtrace.txt
