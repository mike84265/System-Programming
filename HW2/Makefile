CFLAGS = -std=c99 -O3 -Wall
CFLAGS = -std=c99 -g -Wall -DDEBUG
CXX = gcc

%.out: %.c
	@echo "Compling $< ..."
	@$(CXX) $(CFLAGS) $< util.o -o $@

%.o: %.c
	@echo "Compiling $< ..."
	@$(CXX) $(CFLAGS) $< -c -o $@

all:
	@make util.o
	@make big_judge.out
	@make judge.out
	@make player.out

clean:
	rm -f *.o *.out *.FIFO
