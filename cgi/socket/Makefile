all:
	make server.bin
	make -C secure

server.bin: server.c Makefile secure/server.cpp
	g++ -std=gnu++2a -O -o server.bin server.c
	sudo setcap 'cap_net_bind_service=+ep' server.bin

clean:
	rm -f a.out
	rm -f server.bin
	make -C secure clean