CC = g++
CFLAGS = -std=gnu++17 -fmax-errors=1
OBJECTS=rsa_kgen_prof.o
DEPS=rsa_kgen_prof.cpp Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
rsa-keygen: $(OBJECTS) $(DEPS)
	@echo "Linking rsa-keygen"
	$(CC) -o rsa-keygen $(CFLAGS) -O $(OBJECTS) -lcrypto++
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	rsa-keygen

clean:
	rm -f $(OBJECTS)
	rm -f rsa-keygen
	rm -f trace.txt
