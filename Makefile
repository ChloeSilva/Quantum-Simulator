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

# TODO: get this working
# simulation library
simulation: simulation.c
	$(CC) $(CFLAGS) -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

# test_simulation: test_simulation.c simulation.o
# 	$(CC) -o test_simulation $^ $(CFLAGS)

# zx-graph library
zx_graph.o: zx_graph.c zx_graph.h
	$(CC) -c $< $(CFLAGS)

zx_graph_rules.o: zx_graph_rules.c zx_graph_rules.h zx_graph.o
	$(CC) -c $< $(CFLAGS)

test_zx_graph: test_zx_graph.c zx_graph.o
	$(CC) -o test_zx_graph $^ $(CFLAGS) $(CLIBS)

run_test_zx_graph: test_zx_graph
	./test_zx_graph

test_zx_graph_rules: test_zx_graph_rules.c zx_graph_rules.o zx_graph.o
	$(CC) -o test_zx_graph_rules $^ $(CFLAGS) $(CLIBS)

run_test_zx_graph_rules: test_zx_graph_rules
	./test_zx_graph_rules

# circuit library
time_list.o: time_list.c time_list.h
	$(CC) -c $< $(CFLAGS)

circuit.o: circuit.c circuit.h time_list.o
	$(CC) -c $< $(CFLAGS)

test_circuit: test_circuit.c circuit.o time_list.o
	$(CC) -o test_circuit $^ $(CFLAGS) $(CLIBS)

run_test_circuit: test_circuit
	./test_circuit

# simplify
simplify.o: simplify.c zx_graph.o circuit.o time_list.o zx_graph_rules.o
	$(CC) -c $< $(CFLAGS)

test_simplify: test_simplify.c simplify.o zx_graph.o circuit.o time_list.o zx_graph_rules.o
	$(CC) -o test_simplify $^ $(CFLAGS) $(CLIBS)

run_test_simplify: test_simplify
	./test_simplify

# run all tests
run_all_tests: run_test_zx_graph run_test_zx_graph_rules run_test_circuit run_test_simplify 

.PHONY: clean

clean:
	rm simplify test_zx_graph test_zx_graph_rules test_circuit test_simplify *.o