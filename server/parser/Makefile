CC = g++
CFLAGS = -std=c++2a -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=match.h match-ptr.h and.h or.h not.h character.h word.h ciword.h repeat.h optional.h load-on-demand.h rules.h test.h capture.h label.h invoke.h parser.h version.h Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)


parser: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking parser"
	@$(CC) $(CFLAGS) -O -o parser $(OBJECTS)
#	./parser -test
	
debug:	clean
debug:	CFLAGS += -g -D DEBUG
debug: DEBUG = debug
debug: parser

clean:
	rm -f parser
	rm -f *.o
	rm -f trace.txt
	rm -f main

