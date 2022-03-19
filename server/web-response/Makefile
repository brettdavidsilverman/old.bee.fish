CC = g++
CFLAGS = -std=c++2a -fmax-errors=1
OBJECTS=main.o
DEPS=main.cpp web-response.h version.h test.h Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
web-response: $(OBJECTS) $(DEPS)
	@echo "Linking Web Response"
	$(CC) $(CFLAGS) -O -o web-response $(OBJECTS) -lstdc++fs
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	web-response

clean:
	rm -f $(OBJECTS)
	rm -f web-response
	rm -f trace.txt
