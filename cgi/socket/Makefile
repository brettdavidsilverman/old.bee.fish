all:
	make http-server
	make -C secure

http-server: server.cpp config.h Makefile
	g++ -std=gnu++17 -O -o http-server server.cpp -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' http-server

clean:
	rm -f http-server
	rm -f http-server.log
	rm -f https-server.log
	make -C secure clean