CC=gcc
DEPS = transition.h nfa.h dfa.h
OBJ = dfa.o nfa.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

main: $(OBJ)
	gcc -o $@ $^
