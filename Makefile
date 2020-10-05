all:
	cd server && make

debug:
	cd server && make debug
	
clean:
	cd server && make clean

install:
	cd server && make install
	@echo "Check log file in server directory"

