CC = g++
CFLAGS = -std=c++17 -fmax-errors=1 -g
INCLUDE = -I./

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $< -c

parser: parser.o main.o
	$(CC) $(CFLAGS) -O -o parser main.o parser.o

parser.o: parser.h parser.cpp stream.h match.h character.h range.h word.h ciword.h and.h or.h not.h optional.h repeat.h request.h version.h

main.o: main.cpp

clean:
	rm -f parser
	rm -f parser.o
	rm -f main.o
