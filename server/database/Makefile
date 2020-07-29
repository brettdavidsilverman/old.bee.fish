CC = g++
CFLAGS = -std=c++2a -no-pie -fmax-errors=1 -g
OBJECTS=database.o file.o main.o
DEPS=database.h pointer.h file.h
%.o: %.cpp
	$(CC) $(CFLAGS) $< -c
	
database: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking server"
	$(CC) $(CFLAGS) -O -o database $(OBJECTS) -lboost_system -lboost_thread -lpthread
	

clean:
	rm -f $(OBJECTS)
	rm -f database
	rm -f db2.data
	rm -f sorted*