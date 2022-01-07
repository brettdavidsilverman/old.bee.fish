CC = g++
CFLAGS = -std=gnu++17 -fmax-errors=1
OBJECTS=main.o
DEPS=main.cpp web-request.h version.h test.h Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
web-request: $(OBJECTS) $(DEPS)
	@echo "Linking Web Request"
	$(CC) -o web-request $(CFLAGS) -O $(OBJECTS)
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	web-request

clean:
	rm -f $(OBJECTS)
	rm -f web-request
	rm -f trace.txt
