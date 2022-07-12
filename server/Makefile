CC = g++

all:
	cd b-string && $(MAKE) $(DEBUG)
	cd database && $(MAKE) $(DEBUG)
	cd id && $(MAKE) $(DEBUG)
	cd parser && $(MAKE) $(DEBUG)
	cd json && $(MAKE) $(DEBUG)
	cd power-encoding && $(MAKE) $(DEBUG)
	cd web-request && $(MAKE) $(DEBUG)
	cd web-response && $(MAKE) $(DEBUG)
	cd b-script && $(MAKE) $(DEBUG)
	cd https && $(MAKE) $(DEBUG)
	cd web-server && $(MAKE) $(DEBUG)
	cd test && $(MAKE) $(DEBUG) && ./test && echo "Next run make install"

install:
	cd https && make install

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	rm -f log
	make -C b-string clean
	make -C web-request clean
	make -C web-response clean
	make -C https clean
	make -C database clean
	make -C parser clean
	make -C json clean
	make -C b-script clean
	make -C id clean
	make -C power-encoding clean
	make -C cgi clean
	make -C test clean
	make -C web-server clean
	
reset:
	rm -f ~/bee.fish.data
	rm -f ~/bee.fish.log
	rm -f ~/server.log
	rm -f ~/server.err
	rm -f ~/temp/*
	make && make install