CC = g++
INCLUDE = -I./
CFLAGS = -std=c++2a $(INCLUDE) -Wall -fmax-errors=1

OBJECTS=main.o test.o
DEPS=match.h and.h or.h not.h character.h word.h ciword.h repeat.h optional.h load-on-demand.h parser.h version.h Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)


parser: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking parser"
	@$(CC) $(CFLAGS) -O -o parser $(OBJECTS)
	
debug:	clean
debug:	CFLAGS += -g -D DEBUG
debug: DEBUG = debug
debug: parser

clean:
	rm -f parser
	rm -f *.o
	rm -f trace.txt

