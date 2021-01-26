CC = g++
CFLAGS=-std=c++2a -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=json.h string.h number.h array.h object.h blank-space.h version.h ../parser/parser Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

json: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking json"
	@$(CC) $(CFLAGS) -O -o json $(OBJECTS)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	json

clean:
	rm -f *.o
	rm -f json
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt