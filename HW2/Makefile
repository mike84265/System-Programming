CFLAGS = -std=c99 -O3 -Wall
CFLAGS = -std=c99 -g -Wall -DDEBUG
CXX = gcc

%.out: %.c
	@echo "Compling $< ..."
	@$(CXX) $(CFLAGS) $< -o $@

%.o: %.c
	@echo "Compiling $< ..."
	@$(CXX) $(CFLAGS) $< -c -o $@

all:
	@make util.o
	@$(CXX) $(CFLAGS) util.o big_judge.c -o big_judge.out
	@make judge.out
