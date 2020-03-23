server.bin: server.cpp server.h main.cpp Makefile
	g++  -std=c++17 -O -o server.bin server.cpp main.cpp -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' server.bin
	
clean:
	rm -f a.out
	rm -f server.bin
