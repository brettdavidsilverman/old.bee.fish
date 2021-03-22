CC = g++
CFLAGS = -std=c++2a -fmax-errors=1
OBJECTS=main.o
DEPS=id.h main.cpp test.h Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
id: $(OBJECTS) $(DEPS)
	@echo "Linking id"
	$(CC) -o id $(CFLAGS) -O $(OBJECTS) -lcrypto
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug: id

clean:
	rm -f $(OBJECTS)
	rm -f id