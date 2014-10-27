#ifndef NFA_H
#define NFA_H

#include "transition.h"
#include "dfa.h"

/**
 * Defines an NFA. We do not define alphabet, as it is given that the alphabet
 * is {a, b}.
 */
struct nfa {
	char Q[256][256]; // 256 states, each can have 256 character name
	int Qcount; // Number of states
	struct transition* delta; // Transition functions
	int Tcount; // Number of transition functions
	char q0[256]; // Start state
	char F[256][256]; // 256 final states, each can have 256 character name
	int Fcount; // Number of final states
};

/**
 * init_nfa
 * summary: initializes myNfa from regex of size regex_size
 */
void init_nfa(char* regex, size_t regex_size, struct nfa* myNfa);

/**
 * nfa_to_dfa
 * summary: converts NFA myNfa to DFA myDfa
 */
void nfa_to_dfa(struct nfa* myNfa, struct dfa* myDfa);

#endif /* NFA_H */
