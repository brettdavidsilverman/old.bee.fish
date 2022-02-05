CC = g++
CFLAGS = -std=gnu++17 -Wall -fmax-errors=1 -Ofast
OBJECTS = main.o
DEPS=power-encoding.h version.h encoding.h test.h

power-encoding: $(OBJECTS)
	@echo "Linking power-encoding"
	@$(CC) $(CFLAGS) ../parser/match.cpp -c
	@$(CC) $(CFLAGS) -o power-encoding $(OBJECTS) match.o

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	power-encoding

clean:
	rm -f *.o
	rm -f power-encoding
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt