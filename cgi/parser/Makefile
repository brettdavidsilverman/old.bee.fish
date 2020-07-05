CC = g++
CFLAGS = -std=c++17 -fmax-errors=1

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c

parser: parser.o main.o
	rm -f parser
	$(CC) $(CFLAGS) -O -o parser parser.o main.o

parser.o: parser.h parser.cpp match.h character.h range.h word.h

main.o: main.cpp parser.h

clean:
	rm -f parser
	rm -f parser.o
	rm -f main.o
