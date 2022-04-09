CC=g++
CFLAGS=-std=c++17 -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=main.cpp object.h version.h ../parser/parser Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

b-script: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking b-script"
	@$(CC) $(CFLAGS) -O -o b-script $(OBJECTS) -lstdc++fs
#	./b-script -test

../parser/parser:
	cd ../parser && make $(DEBUG)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	b-script

clean:
	rm -f *.o
	rm -f a.out
	rm -f b-script
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
