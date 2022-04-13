CC=gcc
CFLAGS=-std=c99 -Wall -pedantic-errors -DUNIX -g -DDEBUG
CLIBS=-lm

# Compiling simulate

# Check for 32-bit vs 64-bit
PROC_TYPE = $(strip $(shell uname -m | grep 64))
 
# Check for Mac OS
OS = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
DARWIN = $(strip $(findstring DARWIN, $(OS)))

# MacOS System
ifneq ($(DARWIN),)
	CFLAGS += -DMAC
	LIBS=-framework OpenCL

	ifeq ($(PROC_TYPE),)
		CFLAGS+=-arch i386
	else
		CFLAGS+=-arch x86_64
	endif
endif

simulate: simulate.c
	$(CC) $(CFLAGS) -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)


# graph library
zx_graph.o: zx_graph.c zx_graph.h
	$(CC) -c $< $(CFLAGS)

zx_graph_rules.o: zx_graph_rules.c zx_graph_rules.h zx_graph.o
	$(CC) -c $< $(CFLAGS)

# test_zx_graph
test_zx_graph: test_zx_graph.c zx_graph.o
	$(CC) -o test_zx_graph $^ $(CFLAGS) $(CLIBS)

# test_zx_graph_rules
test_zx_graph_rules: test_zx_graph_rules.c zx_graph_rules.o zx_graph.o
	$(CC) -o test_zx_graph_rules $^ $(CFLAGS) $(CLIBS)

# linked list library
linked_list.o: linked_list.c linked_list.h
	$(CC) -c $< $(CFLAGS)

# circuit library
circuit.o: circuit.c circuit.h linked_list.o
	$(CC) -c $< $(CFLAGS)

# test_circuit
test_circuit: test_circuit.c circuit.o linked_list.o
	$(CC) -o test_circuit $^ $(CFLAGS) $(CLIBS)

# simplify
simplify.o: simplify.c zx_graph.o circuit.o linked_list.o
	$(CC) -c $< $(CFLAGS)

# test_simplify
test_simplify: test_simplify.c simplify.o zx_graph.o circuit.o linked_list.o
	$(CC) -o test_simplify $^ $(CFLAGS) $(CLIBS)

.PHONY: clean

clean:
	rm simulate simplify test_zx_graph test_zx_graph_rules test_circuit *.o