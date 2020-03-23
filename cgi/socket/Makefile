all:
	make server.bin
	make -C secure

server.bin: server.cpp Makefile
	g++ -std=gnu++17 -O -o server.bin server.cpp -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' server.bin

clean:
	rm -f server.bin
	rm -f http.log
	rm -f https.log
	make -C secure clean