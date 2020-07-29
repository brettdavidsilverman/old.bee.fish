CC = g++
INCLUDE = -I./
DEBUG =
CFLAGS = -std=c++2a $(INCLUDE) -no-pie -Wall -fmax-errors=1
ifdef DEBUG
	CFLAGS += -g -D DEBUG
endif

# Name of text file containing build number.
BUILD_NUMBER_FILE=build-number.txt
OBJECTS=parser.o main.o test.o
DEPS=match.h and.h or.h not.h character.h word.h ciword.h repeat.h optional.h lazy-load.h json.h ../request.h Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

parser: $(OBJECTS) $(DEPS) $(BUILD_NUMBER_FILE) Makefile
	rm -f parser
	@echo "Linking parser"
	@$(CC) $(CFLAGS) -O -o parser $(OBJECTS) $(BUILD_NUMBER_LDFLAGS)

clean:
	rm -f parser
	rm -f $(OBJECTS)
	rm -f trace.txt

# Include build number rules.
include build-number.mk