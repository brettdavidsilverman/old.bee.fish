CC = g++
CFLAGS = -std=c++2a -fmax-errors=1

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c

parser: parser.o main.o
	rm -f parser
	$(CC) $(CFLAGS) -O -o parser parser.o main.o

parser.o: parser.h parser.cpp stream.h match.h character.h range.h word.h and.h or.h not.h optional.h repeat.h

main.o: main.cpp parser.o

clean:
	rm -f parser
	rm -f parser.o
	rm -f main.o
