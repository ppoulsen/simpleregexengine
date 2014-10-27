#ifndef DFA_H
#define DFA_H

#include "transition.h"

/**
 * Defines a DFA. We do not define alphabet, as it is given that the alphabet
 * is {a, b}.
 */
struct dfa {
	char Q[256][256]; // 256 states, each can have 256 character name
	int Qcount; // Number of states
	struct transition* delta; // Transition functions
	int Tcount; // Number of transition functions
	char q0[256]; // Start state
	char F[256][256]; // 256 final states, each can have 256 character name
	int Fcount; // Number of final states
};

/**
 * init_dfa
 * summary: Creates a DFA from regex of size regex_size
 */
void init_dfa(char* regex, size_t regex_size, struct dfa* myDfa);

/**
 * test_line
 * summary: Tests a line of size line_size against DFA myDfa
 */
int test_line(char* line, size_t line_size, struct dfa* myDfa);
#endif /* DFA_H */
