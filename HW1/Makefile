CFLAGS = -DDEBUG
all: server.c
	gcc server.c $(CFLAGS) -o write_server
	gcc server.c $(CFLAGS) -D READ_SERVER -o read_server

clean:
	rm -f read_server write_server
