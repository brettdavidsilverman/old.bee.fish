CC=g++
CFLAGS=-std=c++2a -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=json.h string.h number.h array.h object.h boolean.h null.h blank-space.h set.h test.h version.h ../parser/parser Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

json: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking json"
	@$(CC) $(CFLAGS) -O -o json $(OBJECTS) -lstdc++fs
#	./json -test

../parser/parser:
	cd ../parser && make $(DEBUG)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	json

time:	clean
time:	CFLAGS += -D TIME -Ofast
time: json

clean:
	rm -f *.o
	rm -f a.out
	rm -f json
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
