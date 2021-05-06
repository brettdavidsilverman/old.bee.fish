CC = g++
CFLAGS = -std=c++2a -Wall -fmax-errors=1

OBJECTS=main.o
DEPS=server.h session.h request.h response.h app.h storage-app.h file-system-app.h test.h authentication.h version.h Makefile config.h storage.h https.h ../parser/parser ../json/json ../database/database ../power-encoding/power-encoding

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

https: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o https $(OBJECTS) -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lpthread -latomic
#	sudo setcap 'cap_net_bind_service=+ep' https

install:
	./start

../parser/parser:
	cd ../parser && make $(DEBUG)
	
../json/json:
	cd ../json && make $(DEBUG)
	
../power-encoding/power-encoding:
	cd ../power-encoding && make $(DEBUG)
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	https

clean:
	rm -f session.log
	rm -f https
	rm -f *.o
	rm -f test.dat
	rm -f trace.txt
#	rm -f ~/bee.fish.data
