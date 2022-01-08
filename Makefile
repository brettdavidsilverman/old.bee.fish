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

feebeecam:	export DEBUG = 0	
feebeecam:	
	cd FeebeeCam && idf.py -B build/release flash -p /dev/ttyS3 -b 1500000

debugfeebeecam:	export DEBUG = 1
debugfeebeecam:
	cd FeebeeCam && idf.py -B build/debug flash -p /dev/ttyS3 -b 1500000

erasefeebeecam:
	cd FeebeeCam && clear && idf.py erase-flash -p /dev/ttyS3 -b 1500000
