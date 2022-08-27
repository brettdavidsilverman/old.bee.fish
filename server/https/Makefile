CC = g++
CFLAGS = -std=c++2a -Wall -fmax-errors=1

OBJECTS=main.o
DEPS=server.h session.h response.h response-headers.h app.h storage-app.h file-system-app.h test.h authentication.h authentication-app.h version.h Makefile ../config.h storage.h https.h ../parser/parser ../json/json ../web-request/web-request ../database/database ../power-encoding/power-encoding  ../web-request/web-request ../b-script/b-script 	Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

https: $(OBJECTS) $(DEPS)
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o https $(OBJECTS) -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lpthread -latomic
	
install:
	sudo setcap 'cap_net_bind_service=+ep' ./https
	./start.sh

../parser/parser:
	cd ../parser && make $(DEBUG)
	
../json/json:
	cd ../json && make $(DEBUG)
	
../power-encoding/power-encoding:
	cd ../power-encoding && make $(DEBUG)

../database/database:
	cd ../database && make $(DEBUG)
	
../web-request/web-request:
	cd ../web-request && make $(DEBUG)

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
