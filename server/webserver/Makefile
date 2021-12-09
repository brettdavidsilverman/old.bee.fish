all: webserver

clean:
	@rm -rf *.o
	@rm -rf webserver

webserver: main.o httpd.o
	g++ -o webserver $^

main.o: main.cpp httpd.h
	g++ -c -o main.o main.cpp

httpd.o: httpd.cpp httpd.h
	g++ -c -o httpd.o httpd.cpp
