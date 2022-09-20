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

# simulation library
simulation.o: simulation.c simulation.h
	$(CC) -c $< $(CFLAGS)

test_simulation: test_simulation.c simulation.o
	$(CC) -o test_simulation $^ $(CFLAGS) $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

run_test_simulation: test_simulation
	./test_simulation

mem_check_simulation: test_simulation
	leaks -atExit -- ./test_simulation

# zx-graph library
zx_graph.o: zx_graph.c zx_graph.h
	$(CC) -c $< $(CFLAGS)

zx_graph_rules.o: zx_graph_rules.c zx_graph_rules.h zx_graph.o
	$(CC) -c $< $(CFLAGS)

test_zx_graph: test_zx_graph.c zx_graph.o
	$(CC) -o test_zx_graph $^ $(CFLAGS) $(CLIBS)

mem_check_zx_graph: test_zx_graph
	leaks -atExit -- ./test_zx_graph

run_test_zx_graph: test_zx_graph
	./test_zx_graph

test_zx_graph_rules: test_zx_graph_rules.c zx_graph_rules.o zx_graph.o
	$(CC) -o test_zx_graph_rules $^ $(CFLAGS) $(CLIBS)

run_test_zx_graph_rules: test_zx_graph_rules
	./test_zx_graph_rules

mem_check_zx_graph_rules: test_zx_graph_rules
	leaks -atExit -- ./test_zx_graph_rules

# circuit library
time_list.o: time_list.c time_list.h
	$(CC) -c $< $(CFLAGS)

circuit.o: circuit.c circuit.h time_list.o
	$(CC) -c $< $(CFLAGS)

test_circuit: test_circuit.c circuit.o time_list.o
	$(CC) -o test_circuit $^ $(CFLAGS) $(CLIBS)

run_test_circuit: test_circuit
	./test_circuit

mem_check_circuit: test_circuit
	leaks -atExit -- ./test_circuit

# simplify
simplify.o: simplify.c zx_graph.o circuit.o time_list.o zx_graph_rules.o circuit_synthesis.o
	$(CC) -c $< $(CFLAGS)

test_simplify: test_simplify.c simplify.o zx_graph.o circuit.o time_list.o zx_graph_rules.o circuit_synthesis.o
	$(CC) -o test_simplify $^ $(CFLAGS) $(CLIBS)

run_test_simplify: test_simplify
	./test_simplify

mem_check_simplify: test_simplify
	leaks -atExit -- ./test_simplify

# circuit synthesis
circuit_synthesis.o: circuit_synthesis.c circuit_synthesis.h zx_graph.o
	$(CC) -c $< $(CFLAGS)

test_circuit_synthesis: test_circuit_synthesis.c circuit_synthesis.o zx_graph.o
	$(CC) -o test_circuit_synthesis $^ $(CFLAGS) $(CLIBS)

run_test_circuit_synthesis: test_circuit_synthesis
	./test_circuit_synthesis

mem_check_circuit_synthesis: test_circuit_synthesis
	leaks -atExit -- ./test_circuit_synthesis

# grover's algorithm
grover: grover.c simulation.o circuit_synthesis.o zx_graph.o
	$(CC) -o $@ $^ $(CFLAGS) $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

# shor's algorithm
shor: shor.c simulation.o
	$(CC) -o $@ $^ $(CFLAGS) $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

# run all tests
run_all_tests: run_test_zx_graph run_test_zx_graph_rules run_test_circuit run_test_simplify run_test_simulation run_test_circuit_synthesis

.PHONY: clean

clean:
	rm test_simulation test_simplify test_zx_graph test_zx_graph_rules test_circuit test_circuit_synthesis grover *.o