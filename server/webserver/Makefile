CFLAGS = -std=c++1z

all: webserver

clean:
	@rm -rf *.o
	@rm -rf webserver

webserver: main.o httpd.o
	g++ -o webserver $^ $(CFLAGS)

main.o: main.cpp httpd.h version.h Makefile
	g++ -c -o main.o main.cpp $(CFLAGS)

httpd.o: httpd.cpp httpd.h version.h Makefile
	g++ -c -o httpd.o httpd.cpp $(CFLAGS)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	webserver
