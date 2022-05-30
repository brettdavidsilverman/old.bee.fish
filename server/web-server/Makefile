CFLAGS = -std=c++1z

all: web-server

clean:
	@rm -rf *.o
	@rm -rf web-server

web-server: main.o
	g++ -pthread -o web-server $^ $(CFLAGS)

main.o: main.cpp web-server.h web-server-base.h version.h Makefile ../web-request/web-request.h
	g++ -c -o main.o main.cpp $(CFLAGS)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	web-server
