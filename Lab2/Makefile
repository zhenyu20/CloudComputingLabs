all: httpserver client
LIBS = -pthread #-lsocket
httpserver: server.c
	gcc -g -W -Wall $(LIBS) -o $@ $<

client: simpleclient.c
	gcc -W -Wall -o $@ $<
clean:
	rm httpserver
