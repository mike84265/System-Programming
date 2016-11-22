# B03901078 蔡承佑
CFLAGS = -std=c99 -g -Wall -DDEBUG=3
CFLAGS = -std=c99 -O3 -Wall
CXX = gcc

main:
	@make util.o
	@make big_judge
	@make judge
	@make player
%.out: %.c
	@echo "Compling $< ..."
	@$(CXX) $(CFLAGS) $< util.o -o $@

big_judge: big_judge.c util.o
	@echo "Compiling big_judge..."
	@$(CXX) $(CFLAGS) big_judge.c util.o -o big_judge

judge: judge.c util.o
	@echo "Compiling judge..."
	@$(CXX) $(CFLAGS) judge.c util.o -o judge

player: player.c util.o
	@echo "Compiling player..."
	@$(CXX) $(CFLAGS) player.c util.o -o player

%.o: %.c
	@echo "Compiling $< ..."
	@$(CXX) $(CFLAGS) $< -c -o $@


clean:
	rm -f *.o *.FIFO
	rm -f big_judge
	rm -f judge
	rm -f player
