#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct transition {
	char currentState[256];
	char input;
	char finalState[256];
};

struct nfa {
	char Q[256][256];
	int Qcount;
	struct transition* delta;
	int Tcount;
	char q0[256];
	char F[256][256];
	int Fcount;
};

struct dfa {
	char Q[256][256];
	int Qcount;
	struct transition* delta;
	int Tcount;
	char q0[256];
	char F[256][256];
	int Fcount;
};

void init_dfa(char* regex, size_t regex_size, struct dfa* myDfa);
int test_line(char* line, size_t line_size, struct dfa* myDfa);

void main (void) {
	char *regex = NULL;
	char *line = NULL;
	size_t regex_size;
	size_t line_size;
	struct dfa myDfa;


	getline(&regex, &regex_size, stdin);
	init_dfa(regex, regex_size, &myDfa);
	
	while(getline(&line, &line_size, stdin) >= 0) {
		if(test_line(line, line_size, &myDfa))
			printf("yes\n");
		else
			printf("no\n");
	}

}

void single_char_nfa(char c, struct nfa* newNfa) {
	newNfa->Qcount = 2;
	strcpy(newNfa->Q[0], "A");
	strcpy(newNfa->Q[1], "B");
	strcpy(newNfa->F[0], "B");
	newNfa->Fcount = 1;
	newNfa->delta = malloc(sizeof(struct transition));
	strcpy(newNfa->delta->currentState, "A");
	strcpy(newNfa->delta->finalState, "B");
	newNfa->delta->input = c;
	newNfa->Tcount = 1;
	strcpy(newNfa->q0, "A");
}

void empty_char_nfa(struct nfa* newNfa) {
	newNfa->Qcount = 1;
	strcpy(newNfa->Q[0], "A");;
	strcpy(newNfa->F[0], "A");
	newNfa->Fcount = 1;
	newNfa->delta = NULL;
	newNfa->Tcount = 0;
	strcpy(newNfa->q0, "A");
}

void union_nfa(struct nfa* nfa1, struct nfa* nfa2, struct nfa* newNfa) {
	int i;
	
	// Set number of states to states from nfa1, nfa2, plus one additional
	newNfa->Qcount = nfa1->Qcount + nfa2->Qcount + 1;
	
	// Copy states from nfa1 and nfa2 to newNfa, prepending to avoid overlap
	for (i = 0; i < newNfa->Qcount - 1; i++) {
		if (i < nfa1->Qcount) {
			strcpy(newNfa->Q[i], "A");
			strcat(newNfa->Q[i], nfa1->Q[i]);
		} else {
			strcpy(newNfa->Q[i], "B");
			strcat(newNfa->Q[i], nfa2->Q[i - nfa1->Qcount]);
		}
	}
	
	// Add new state to be initial state
	strcpy(newNfa->Q[i], "S");
	
	// Set final state count to sum of nfa1 and nfa2
	newNfa->Fcount = nfa1->Fcount + nfa2->Fcount;
	
	// Copy final states from nfa1 and nfa2 to newNfa
	for (i = 0; i < newNfa->Fcount; i++) {
		if (i < nfa1->Fcount) {
			strcpy(newNfa->F[i], "A");
			strcat(newNfa->F[i], nfa1->F[i]);
		} else {
			strcpy(newNfa->F[i], "B");
			strcat(newNfa->F[i], nfa2->F[i - nfa1->Fcount]);
		}
	}
	
	// Set number of transitions equal to nfa1 and nfa2, plus two
	// One for transition from new state to nfa1 start state
	// And one for transition from new state to nfa2 start state
	newNfa->Tcount = nfa1->Tcount + nfa2->Tcount + 2;
	
	// Malloc and copy states over
	newNfa->delta = malloc(sizeof(struct transition) * newNfa->Tcount);
	for (i = 0; i < newNfa->Tcount - 2; i++) {
		if (i < nfa1->Tcount) {
			// Keep same input
			newNfa->delta[i] = (struct transition){
				.input = nfa1->delta[i].input,
			};
			// Prepend A to states to avoid collision
			strcpy(newNfa->delta[i].currentState, "A");
			strcat(newNfa->delta[i].currentState, nfa1->delta[i].currentState);
			strcpy(newNfa->delta[i].finalState, "A");
			strcat(newNfa->delta[i].finalState, nfa1->delta[i].finalState);
		} else {
			// Keep same input
			newNfa->delta[i] = (struct transition){
				.input = nfa2->delta[i - nfa1->Tcount].input,
			};
			
			// Prepend A to states to avoid collision
			strcpy(newNfa->delta[i].currentState, "B");
			strcat(newNfa->delta[i].currentState, nfa2->delta[i - nfa1->Tcount].currentState);
			strcpy(newNfa->delta[i].finalState, "B");
			strcat(newNfa->delta[i].finalState, nfa2->delta[i - nfa1->Tcount].finalState);
		}
	}
	
	// Add transitions from S to nfa1->q0 and nfa2->q0
	newNfa->delta[i] = (struct transition){
		.input = 'e',
	};
	strcpy(newNfa->delta[i].currentState, "S");
	strcpy(newNfa->delta[i].finalState, "A");
	strcat(newNfa->delta[i].finalState, nfa1->q0);
	newNfa->delta[i+1] = (struct transition){
		.input = 'e',
	};
	strcpy(newNfa->delta[i+1].currentState, "S");
	strcpy(newNfa->delta[i+1].finalState, "B");
	strcat(newNfa->delta[i+1].finalState, nfa2->q0);
	
	// Set S to start state
	strcpy(newNfa->q0, "S");
	
	// Free unused transition tables
	free(nfa1->delta);
	free(nfa2->delta);
}

void concat_nfa(struct nfa* nfa1, struct nfa* nfa2, struct nfa* newNfa) {
	int i;
	
	// Same number of states, so just sum
	newNfa->Qcount = nfa1->Qcount + nfa2->Qcount;
	
	// Copy states over, with prepending to avoid collision
	for (i = 0; i < newNfa->Qcount; i++) {
		if (i < nfa1->Qcount) {
			strcpy(newNfa->Q[i], "A");
			strcat(newNfa->Q[i], nfa1->Q[i]);
		} else {
			strcpy(newNfa->Q[i], "B");
			strcat(newNfa->Q[i], nfa2->Q[i - nfa1->Qcount]);
		}
	}
	
	// Only nfa2's final states are copied
	newNfa->Fcount = nfa2->Fcount;
	
	// Copy nfa2 final states
	for (i = 0; i < newNfa->Fcount; i++) {
		strcpy(newNfa->F[i], "B");
		strcat(newNfa->F[i], nfa2->F[i]);
	}

	// Same transitions as before, + 1 for each nfa1 final state
	newNfa->Tcount = nfa1->Tcount + nfa2->Tcount + nfa1->Fcount;
	
	// Malloc and copy original transitions over
	newNfa->delta = malloc(sizeof(struct transition) * newNfa->Tcount);
	for (i = 0; i < newNfa->Tcount - nfa1->Fcount; i++) {
		if (i < nfa1->Tcount) {
			newNfa->delta[i] = (struct transition){
				.input = nfa1->delta[i].input,
			};
			strcpy(newNfa->delta[i].currentState, "A");
			strcat(newNfa->delta[i].currentState, nfa1->delta[i].currentState);
			strcpy(newNfa->delta[i].finalState, "A");
			strcat(newNfa->delta[i].finalState, nfa1->delta[i].finalState);
		} else {
			newNfa->delta[i] = (struct transition){
				.input = nfa2->delta[i - nfa1->Tcount].input,
			};
			strcpy(newNfa->delta[i].currentState, "B");
			strcat(newNfa->delta[i].currentState, nfa2->delta[i - nfa1->Tcount].currentState);
			strcpy(newNfa->delta[i].finalState, "B");
			strcat(newNfa->delta[i].finalState, nfa2->delta[i - nfa1->Tcount].finalState);
		}
	}
	
	// Create new transition from each nfa1->F to nfa2->q0
	for ( ; i < newNfa->Tcount; i++) {
		newNfa->delta[i] = (struct transition){
			.input = 'e',
		};
		strcpy(newNfa->delta[i].currentState, "A"); 
		strcat(newNfa->delta[i].currentState, nfa1->F[i - nfa1->Tcount - nfa2->Tcount]); 
		strcpy(newNfa->delta[i].finalState, "B");
		strcat(newNfa->delta[i].finalState, nfa2->q0);
	};
	
	// New Start state is nfa1 start state
	strcpy(newNfa->q0, "A");
	strcat(newNfa->q0, nfa1->q0);
	
	// Free unused transitions
	free(nfa1->delta);
	free(nfa2->delta);
}

void star_nfa(struct nfa* nfa1, struct nfa* newNfa) {
	int i;
	
	// Add one state to existing
	newNfa->Qcount = nfa1->Qcount + 1;
	
	// Copy states over, with prepending to avoid collision
	for (i = 0; i < newNfa->Qcount; i++) {
		if (i < nfa1->Qcount) {
			strcpy(newNfa->Q[i], "A");
			strcat(newNfa->Q[i], nfa1->Q[i]);
		}
	}
	
	// Create new state
	strcpy(newNfa->Q[i], "S");
	
	// Add the new state to existing final states
	newNfa->Fcount = nfa1->Fcount + 1;
	
	// Copy nfa1 final states
	for (i = 0; i < newNfa->Fcount - 1; i++) {
		strcpy(newNfa->F[i], "A");
		strcat(newNfa->F[i], nfa1->F[i]);
	}
	
	// Add new state to final states
	strcpy(newNfa->F[i], "S");

	// Same transitions as before, + 1 from new state to old start state
	// And + 1 for each Final state to old start state
	newNfa->Tcount = nfa1->Tcount + nfa1->Fcount + 1;
	
	// Malloc and copy original transitions over
	newNfa->delta = malloc(sizeof(struct transition) * newNfa->Tcount);
	for (i = 0; i < nfa1->Tcount; i++) {
		newNfa->delta[i] = (struct transition){
			.input = nfa1->delta[i].input,
		};
		strcpy(newNfa->delta[i].currentState, "A");
		strcat(newNfa->delta[i].currentState, nfa1->delta[i].currentState);
		strcpy(newNfa->delta[i].finalState, "A");
		strcat(newNfa->delta[i].finalState, nfa1->delta[i].finalState);
	}
	
	// Create new transition from each nfa1->F to nfa1->q0
	for ( ; i < newNfa->Tcount - 1; i++) {
		newNfa->delta[i] = (struct transition){
			.input = 'e',
		};
		strcpy(newNfa->delta[i].currentState, "A"); 
		strcat(newNfa->delta[i].currentState, nfa1->F[i - nfa1->Tcount]); 
		strcpy(newNfa->delta[i].finalState, "A");
		strcat(newNfa->delta[i].finalState, nfa1->q0);
	};
	
	// Create one more transition from new state S to old start state
	newNfa->delta[i] = (struct transition){
		.input = 'e',
	};
	strcpy(newNfa->delta[i].currentState, "S");
	strcpy(newNfa->delta[i].finalState, "A");
	strcat(newNfa->delta[i].finalState, nfa1->q0);
	
	// New Start state is S
	strcpy(newNfa->q0, "S");
	
	// Free unused transitions
	free(nfa1->delta);
}

void init_nfa(char* regex, size_t regex_size, struct nfa* myNfa) {
	
}

void init_dfa(char* regex, size_t regex_size, struct dfa* myDfa) {
	struct nfa myNfa;
	init_nfa(regex, regex_size, &myNfa);
}

int test_line(char* line, size_t line_size, struct dfa* myDfa) {
	return 0;
}
