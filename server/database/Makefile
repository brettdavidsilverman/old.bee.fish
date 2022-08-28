CC = g++
CFLAGS = -std=c++2a -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=database.h path.h file.h index.h branch.h config.h map.h test.h version.h ../power-encoding/power-encoding.h version.h
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $< -c
	
database: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking database"
	$(CC) $(CFLAGS) -O -o database $(OBJECTS) -lboost_system -lboost_thread -lboost_chrono -lpthread -latomic
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	database

clean:
	rm -f $(OBJECTS)
	rm -f database
	rm -f data
	rm -f test.data
	rm -f trace.txt
	rm -f sorted*
	rm -f tests/*.txt
	rm -f tests/*.tar
	rm -f tests/data
