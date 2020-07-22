CC = g++
CFLAGS = -std=c++2a -no-pie -fmax-errors=1 -g
OBJECTS=database.o file.o main.o

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c

database: database.cpp database.h Makefile

database: $(OBJECTS) Makefile
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o database $(OBJECTS) -lboost_system -lboost_thread -lpthread
	

file: file.cpp file.h Makefile

main: main.cpp file.o database.o

clean:
	rm -f database.o
	rm -f main.o
	rm -f file.o
	rm -f database
	rm -f db2.data
	rm -f sorted*