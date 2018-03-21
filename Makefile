.PHONY: clean

CC=gcc
CFLAGS=-O0 -ggdb $(shell pkg-config --cflags libbsd-overlay)
LDFLAGS=$(shell pkg-config --libs libbsd-overlay) 

all: server

server: server.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f server server.o
