CC = g++
CFLAGS = -std=c++2a -Wall -fmax-errors=1

OBJECTS=session.o response.o main.o server.o md5.o wstring.o
DEPS=server.h session.h request.h response.h authentication.h md5.h wstring.h base64.h basic-authorization.h version.h Makefile config.h ../database/database ../power-encoding/power-encoding

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
	rm -f trace.txt
	rm -f ~/bee.fish.data
