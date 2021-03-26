CC = g++
CFLAGS = -std=c++2a -fmax-errors=1
OBJECTS=main.o
DEPS=utf-8.h char.h b-string.h bit-string.h data.h base64.h string.h test.h main.cpp Makefile

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
