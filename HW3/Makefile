# B03901078 蔡承佑
CFLAGS = -std=c99 -g -Wall -DDEBUG=4
CFLAGS = -std=c99 -O3 -Wall
CXX = gcc

main:

%.out: %.c
	@echo "Compling $< ..."
	@$(CXX) $(CFLAGS) $< util.o -o $@

%.o: %.c
	@echo "Compiling $< ..."
	@$(CXX) $(CFLAGS) $< -c -o $@


clean:
	rm -f *.o *.out
