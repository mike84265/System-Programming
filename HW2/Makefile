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
	@echo "Compiling big_judge..."
	@$(CXX) $(CFLAGS) util.o big_judge.c -o big_judge.out
	@echo "Compiling judge..."
	@$(CXX) $(CFLAGS) util.o judge.c -o judge.out
	@echo "Compiling player..."
	@$(CXX) $(CFLAGS) util.o player.c -o player.out
