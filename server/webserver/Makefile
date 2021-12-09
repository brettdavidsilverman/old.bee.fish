CXXFLAGS = -std=c++1z

all: webserver

clean:
	@rm -rf *.o
	@rm -rf webserver

webserver: main.o httpd.o
	g++ -o webserver $^ $(CXXFLAGS)

main.o: main.cpp httpd.h version.h Makefile
	g++ -c -o main.o main.cpp $(CXXFLAGS)

httpd.o: httpd.cpp httpd.h version.h Makefile
	g++ -c -o httpd.o httpd.cpp $(CXXFLAGS)
