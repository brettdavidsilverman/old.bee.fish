https-server-asio: server.h server.cpp session.h session.cpp https_session.h https_session.cpp Makefile
	g++  -std=c++17 -O -o https-server-asio session.cpp https_session.cpp server.cpp -lssl -lcrypto -lboost_system -lboost_thread -lpthread
	sudo setcap 'cap_net_bind_service=+ep' https-server-asio
	
clean:
	rm -f https-server-asio
