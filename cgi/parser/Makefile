CC = g++
CFLAGS = -std=c++20 -fmax-errors=1 -g
INCLUDE = -I./

# Name of text file containing build number.
BUILD_NUMBER_FILE=build-number.txt
OBJECTS=parser.o main.o test.o

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -c

parser: $(OBJECTS) $(BUILD_NUMBER_FILE)
	rm -f parser
	@echo parser
	@$(CC) $(CFLAGS) -O -o parser main.o parser.o test.o $(BUILD_NUMBER_LDFLAGS)

parser.o: parser.h parser.cpp match.h character.h range.h word.h ciword.h and.h or.h not.h optional.h repeat.h request.h version.h

main.o: main.cpp request.h version.h parser.o test.o

test.o: parser.h test.cpp parser.o

clean:
	rm -f parser
	rm -f parser.o
	rm -f main.o
	rm -f test.o
	rm -f trace.txt

# Include build number rules.
include build-number.mak