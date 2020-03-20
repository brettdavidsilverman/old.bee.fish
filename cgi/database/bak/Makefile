main.bin: main.cpp database.o file.o Makefile
	g++ -std=c++2a -O -o main.bin database.o file.o main.cpp -lboost_system -lboost_thread -lpthread

file.o: file.h file.cpp Makefile
	g++ -std=c++2a -O -c -o file.o file.cpp
	
database.o: database.cpp database.h file.o Makefile
	g++ -std=c++2a -O -c -o database.o database.cpp
	rm -f db.b2
	#cat /usr/share/dict/words | ./main.bin 1
clean:
	rm -f file.o
	rm -f database.o
	rm -f main.bin
	rm -f db.b2
	