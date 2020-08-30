all:
	cd server && make

debug:
	cd server && make debug
	
clean:
	cd server && make clean

install:
	./start
	@echo "Check log file in this directory"

