CC = g++
PARSER = ./parser
DATABASE = ./database
ID = ./id
HTTPS = ./https

all:
	cd b-string && $(MAKE) $(DEBUG)
	cd database && $(MAKE) $(DEBUG)
	cd id && $(MAKE) $(DEBUG)
	cd parser && $(MAKE) $(DEBUG)
	cd json && $(MAKE) $(DEBUG)
	cd power-encoding && $(MAKE) $(DEBUG)
	cd https && $(MAKE) $(DEBUG)
	cd test && $(MAKE) $(DEBUG) && ./test
	
install:
	cd https && make install

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	rm -f log
	make -C b-string clean
	make -C https clean
	make -C database clean
	make -C parser clean
	make -C json clean
	make -C id clean
	make -C power-encoding clean
	make -C cgi clean
	make -C test clean

clear:
	rm -f ~/bee.fish.data
	rm -f ~/bee.fish.log
	make && make install

