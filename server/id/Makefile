CC = g++
CFLAGS = -std=c++2a -fmax-errors=1
OBJECTS=main.o
DEPS=id.h main.cpp
%.o: %.cpp
	$(CC) $< -c $(CFLAGS)
	
id: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking id"
	rm -f id
	$(CC) -o id $(CFLAGS) -O $(OBJECTS)
	
debug:	CFLAGS += -g -D DEBUG
debug: id

clean:
	rm -f $(OBJECTS)
	rm -f id