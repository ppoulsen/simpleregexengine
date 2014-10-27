#include <string.h>
#include "dfa.h"
#include "nfa.h"

void init_dfa(char* regex, size_t regex_size, struct dfa* myDfa) {
	struct nfa myNfa;
	init_nfa(regex, regex_size, &myNfa);
	nfa_to_dfa(&myNfa, myDfa);
}

int test_line(char* line, size_t line_size, struct dfa* myDfa) {
	int i;
	char curState[256];
	int is_final = 0;
	// Start curState at start state q0
	strcpy(curState, myDfa->q0);

	// Iterate through each character in line
	for (i = 0; i < line_size; i++) {
		int j;
		char c = line[i];
		
		// Find transition function that matches input and currentState
		for (j=0; j<myDfa->Tcount; j++) {
			if (!strcmp(curState, myDfa->delta[j].currentState) && c == myDfa->delta[j].input) {
				// Set curState to destination finalState
				strcpy(curState, myDfa->delta[j].finalState);
				break;
			}
		}
	}
	
	// Iterate through final states to determine if this was successful
	for (i = 0; i < myDfa->Fcount; i++) {
		if (!strcmp(curState, myDfa->F[i])) {
			is_final = 1;
			break;
		}
	}
	return is_final;
}
