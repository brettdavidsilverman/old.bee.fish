CC = g++
INCLUDE = -I./
CFLAGS = -std=c++2a $(INCLUDE) -no-pie -Wall -fmax-errors=1

# Name of text file containing build number.
BUILD_NUMBER_FILE=../build-number.txt
OBJECTS=main.o test.o
DEPS=match.h and.h or.h not.h character.h word.h ciword.h repeat.h optional.h lazy-load.h json/json.h json/string.h json/number.h json/array.h json/object.h json/blank-space.h ../request.h Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

parser: $(OBJECTS) $(DEPS) $(BUILD_NUMBER_FILE) Makefile
	cd json && make $(DEBUG)
	rm -f parser
	@echo "Linking parser"
	@$(CC) $(CFLAGS) -O -o parser $(OBJECTS) json/json.o json/object.o json/array.o $(BUILD_NUMBER_LDFLAGS)
	
	
debug:	clean
debug:	CFLAGS += -g -D DEBUG
debug: DEBUG = debug
debug: parser

clean:
	rm -f parser
	rm -f $(OBJECTS)
	rm -f trace.txt
	cd json && make clean

# Include build number rules.
include ../build-number.mk