CC = g++
PARSER = ./parser
DATABASE = ./database
ID = ./id
HTTPS = ./https

all:
	cd database && $(MAKE) $(DEBUG)
	cd id && $(MAKE) $(DEBUG)
	cd parser && $(MAKE) $(DEBUG)
	cd power-encoding && $(MAKE) $(DEBUG)
	cd https && $(MAKE) $(DEBUG)
	
install:
	cd https && make install
	
debug: DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	rm -f log
	rm -f bee.fish.data
	make -C https clean
	make -C database clean
	make -C parser clean
	make -C id clean
	make -C power-encoding clean
