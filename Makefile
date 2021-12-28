all:
	cd server && make

debug:
	cd server && make debug
	
clean:
	cd server && make clean

reset:
	cd server && make reset

install:
	cd server && make install

feebeecam:
	cd FeebeeCam && clear && idf.py flash -p /dev/ttyS3 -b 1500000
