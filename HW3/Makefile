# B03901078 蔡承佑
CFLAGS = -std=c99 -g -Wall -DDEBUG
CFLAGS = -std=c99 -O3 -Wall
CXX = gcc

main: server.c file_reader.c util.c main.c
	@make normal
	@make slow
	@make util.o
	@make server.o
	@gcc $(CFLAGS) util.o server.o main.c -o server.out

slow:
	@echo "Compiling slow_reader ..."
	@$(CXX) $(CFLAGS) -DSLOW file_reader.c -o slow_reader

normal:
	@echo "Compiling file_reader ..."
	@$(CXX) $(CFLAGS) file_reader.c -o file_reader

%.out: %.c
	@echo "Compiling $< ..."
	@$(CXX) $(CFLAGS) $< -o $@

%.o: %.c
	@echo "Compiling $< ..."
	@$(CXX) $(CFLAGS) $< -c -o $@


clean:
	@rm -f *.o *.out
	@rm file_reader
	@rm slow_reader
