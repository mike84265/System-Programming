CFLAGS = -DDEBUG 
all: server.c
	gcc util.c -c -o util.o
	gcc server.c $(CFLAGS) util.o -o write_server
	gcc server.c $(CFLAGS) util.o -D READ_SERVER -o read_server

clean:
	rm -f read_server write_server
