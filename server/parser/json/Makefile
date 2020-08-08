CC = g++
INCLUDE = -I../
CFLAGS = -std=c++2a $(INCLUDE) -no-pie -Wall -fmax-errors=1 -Ofast
OBJECTS=json.o object.o main.o array.o
BUILD_NUMBER_FILE=../../build-number.txt
DEPS=json.h string.h number.h array.h object.h blank-space.h Makefile

json: $(OBJECTS)
	rm -f json
	@echo "Linking json"
	@$(CC) $(CFLAGS) -O -o json $(OBJECTS) $(BUILD_NUMBER_LDFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	json

clean:
	rm -f *.o
	rm -f json
	rm -f tmp
	
	
include ../../build-number.mk