CC = g++
INCLUDE = -I../
CFLAGS = -std=c++2a $(INCLUDE) -no-pie -Wall -fmax-errors=1
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
debug:	CFLAGS += -g -D DEBUG
debug:	json

clean:
	rm -f $(OBJECTS)
	
	
include ../../build-number.mk