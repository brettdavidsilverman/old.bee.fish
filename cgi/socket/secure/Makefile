https-server: server.cpp server.h main.cpp Makefile
	make -C asio
	g++  -std=c++17 -O -o https-server server.cpp main.cpp -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' https-server
	
clean:
	rm -f https-server.log
	rm -f https-server
	make -C asio clean
