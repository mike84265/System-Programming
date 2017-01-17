CXX = gcc
CFLAGS = -std=c99 -Wall -g -O3

main:
	$(CXX) $(CFLAGS) main.c util.c -o main.out

debug:
	$(CXX) $(CFLAGS) -DDEBUG main.c util.c -o main.out

clean:
	rm -f *.out
