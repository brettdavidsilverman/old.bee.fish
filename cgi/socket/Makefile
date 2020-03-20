server.bin: server.c Makefile
	g++ -std=gnu++2a -O -o server.bin server.c
	sudo setcap 'cap_net_bind_service=+ep' server.bin
	make -C secure
	
clean:
	rm -f a.out
	rm -f server.bin
	make -C secure clean