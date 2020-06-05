main.bin: main.cpp database.o file.o Makefile
	g++ -std=c++2a -O -o main.bin database.o file.o main.cpp -lboost_system -lboost_thread -lpthread

database.o: database.cpp database.h Makefile
	g++ -std=c++2a -O -c -o database.o database.cpp
	
file.o: file.cpp file.h Makefile
	g++ -std=c++2a -O -c -o file.o file.cpp

clean:
	rm -f database.o
	rm -f file.o
	rm -f main.bin
	rm -f db2.data
	rm -f sorted*