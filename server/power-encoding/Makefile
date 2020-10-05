CC = g++
INCLUDE = -I../
CFLAGS = -std=c++2a $(INCLUDE) -no-pie -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=power-encoding.h version.h

power-encoding: $(OBJECTS)
	rm -f power-encoding
	@echo "Linking power-encoding"
	@$(CC) $(CFLAGS) -o power-encoding $(OBJECTS)

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	power-encoding

char-bits:	clean
char-bits:	CFLAGS += -D GEN_CHAR_BITS
char-bits:	power-encoding
char-bits:	./power-encoding

clean:
	rm -f *.o
	rm -f power-encoding
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt