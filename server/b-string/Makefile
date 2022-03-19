CC = g++
CFLAGS = -std=gnu++17 -Wall -fmax-errors=1
OBJECTS=main.o
DEPS=character.h b-string.h bit-stream.h data.h base64.h misc.h string.h stream.h test.h main.cpp Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
b-string: $(OBJECTS) $(DEPS)
	@echo "Linking b-string"
	$(CC) -o b-string $(CFLAGS) -O $(OBJECTS) -lcrypto
	
	
debug:	CFLAGS += -g -D DEBUG
debug:	clean
debug: b-string

clean:
	rm -f $(OBJECTS)
	rm -f b-string
