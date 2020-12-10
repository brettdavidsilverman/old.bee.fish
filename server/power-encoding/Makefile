CC = g++
INCLUDE = -I../
CFLAGS = -std=c++2a $(INCLUDE) -Wall -fmax-errors=1 -Ofast
OBJECTS = test.o main.o
DEPS=power-encoding.h version.h encoding.h

power-encoding: $(OBJECTS)
	rm -f power-encoding
	@echo "Linking power-encoding"
	@$(CC) $(CFLAGS) -o power-encoding $(OBJECTS) -lcrypto

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