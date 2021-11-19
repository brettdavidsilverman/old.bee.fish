CC = g++
CFLAGS = -std=gnu++20 -fmax-errors=1
OBJECTS=main.o
DEPS=main.cpp test.h test-suite.h version.h ../b-string/b-string ../parser/parser ../json/json ../https/https Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
test: $(OBJECTS) $(DEPS)
	@echo "Linking test"
	$(CC) -o test $(CFLAGS) -O $(OBJECTS) -lcrypto -lstdc++fs

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug: test

clean:
	rm -f $(OBJECTS)
	rm -f test