CC=gcc
CFLAGS=-Wall -std=c99 -pedantic-errors
CLIBS=-lm

# graph library
zx_graph.o: zx_graph.c zx_graph.h
	$(CC) -c $< $(CFLAGS)

# simplify
simplify: simplify.c zx_graph.o
	$(CC) -o simplify $< $(CFLAGS) $(CLIBS)