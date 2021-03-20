CC = g++
CFLAGS = -std=c++2a -fmax-errors=1
OBJECTS=main.o
DEPS=utf-8.h b-string.h test.h main.cpp Makefile

%.o: %.cpp
	$(CC) $< -c $(CFLAGS)
	
b-string: $(OBJECTS) $(DEPS)
	@echo "Linking b-string"
	$(CC) -o b-string $(CFLAGS) -O $(OBJECTS)
	
debug:	CFLAGS += -g -D DEBUG
debug: b-string

clean:
	rm -f $(OBJECTS)
	rm -f b-string