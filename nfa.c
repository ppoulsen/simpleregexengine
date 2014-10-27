#include <stdlib.h>
#include <string.h>

#include "nfa.h"
#include "dfa.h"

/**
 * single_char_nfa
 * summary: Construct an NFA that only accepts a one character string of c
 */
void single_char_nfa(char c, struct nfa* newNfa) {
	// Need two states
	newNfa->Qcount = 2;
	strcpy(newNfa->Q[0], "A");
	strcpy(newNfa->Q[1], "B");
	
	// Second state is final
	strcpy(newNfa->F[0], "B");
	newNfa->Fcount = 1;
	
	// Single transition from A to B on character c
	newNfa->delta = malloc(sizeof(struct transition));
	strcpy(newNfa->delta->currentState, "A");
	strcpy(newNfa->delta->finalState, "B");
	newNfa->delta->input = c;
	newNfa->Tcount = 1;
	
	// Start is A
	strcpy(newNfa->q0, "A");
}

/**
 * empty_char_nfa
 * summary: Construct an NFA that only accepts the empty string
 */
void empty_char_nfa(struct nfa* newNfa) {
	// Single state
	newNfa->Qcount = 1;
	strcpy(newNfa->Q[0], "A");
	
	// It's also the final state
	strcpy(newNfa->F[0], "A");
	newNfa->Fcount = 1;
	
	// No transitions
	newNfa->delta = NULL;
	newNfa->Tcount = 0;
	strcpy(newNfa->q0, "A");
}

/**
 * union_nfa
 * summary: Construct an NFA that is the union of nfa1 and nfa2
 */
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

/**
 * concat_nfa
 * summary: Construct an NFA that is the concatenation of nfa1 and nfa2
 */
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
	for (i = 0; i < nfa1->Qcount; i++) {
		strcpy(newNfa->Q[i], "A");
		strcat(newNfa->Q[i], nfa1->Q[i]);
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

/**
 * copy_nfa
 * summary: Construct an NFA that is exactly the same as nfa1
 */
void copy_nfa(struct nfa* nfa1, struct nfa* newNfa) {
	int i;
	
	// Same number of states
	newNfa->Qcount = nfa1->Qcount;
	
	// Copy states over
	for (i = 0; i < newNfa->Qcount; i++) {
		strcpy(newNfa->Q[i], nfa1->Q[i]);
	}
	
	// Same number of final states
	newNfa->Fcount = nfa1->Fcount;
	
	// Copy nfa1 final states
	for (i = 0; i < newNfa->Fcount; i++) {
		strcpy(newNfa->F[i], nfa1->F[i]);
	}
	
	// Same transitions as before
	newNfa->Tcount = nfa1->Tcount;
	
	// Malloc and copy original transitions over
	newNfa->delta = malloc(sizeof(struct transition) * newNfa->Tcount);
	for (i = 0; i < nfa1->Tcount; i++) {
		newNfa->delta[i] = (struct transition){
			.input = nfa1->delta[i].input,
		};
		strcpy(newNfa->delta[i].currentState, nfa1->delta[i].currentState);
		strcpy(newNfa->delta[i].finalState, nfa1->delta[i].finalState);
	}
	
	// Copy start state
	strcpy(newNfa->q0, nfa1->q0);
	
	// Free unused transitions
	free(nfa1->delta);
}

/**
 * vertical_extract
 * summary: Find the substring that ought to be resolved before applying union operator
 */
void vertical_extract(char* regex, size_t regex_size, size_t *substr_size) {
	int bar_count = 1;
	int bar_ignore = 0;
	*substr_size = 0;
	
	// First character must be |
	if(regex[0] != '|')
		return;
	
	// Find next vertical bar or end of string
	while(bar_count > 0) {
		(*substr_size)++;
		if ((*substr_size) == regex_size && !bar_ignore)
			return;
		if (regex[*substr_size] == '|') {
			bar_count--;
		} else if (regex[*substr_size] == '(') {
			bar_ignore++; // Ignore bars inside ()
		} else if (regex[*substr_size] == ')') {
			bar_ignore--; // Stop ignore once outside
		}
	}

	// Substract the size of the first vertical bar
	*substr_size -= 1;
}

/**
 * paran_extract
 * summary: Find the substring that ought to be resolved for paranthetical grouping
 */
void paran_extract(char* regex, size_t regex_size, size_t *substr_size, int *has_star) {
	int paren_count = 1;
	*substr_size = 0;
	*has_star = 0;
	
	// First character must be (
	if(regex[0] != '(')
		return;
	
	// Find paired )
	while(paren_count > 0) {
		(*substr_size)++;
		if (regex[*substr_size] == '(') {
			paren_count++;
		} else if (regex[*substr_size] == ')') {
			paren_count--;
		}
	}
	
	// If star is after, let calling function know
	if (*substr_size + 1 < regex_size) {
		if (regex[*substr_size + 1] == '*') {
			*has_star = 1;
		}
	}
	
	// Subtract the size of left paranthesis
	*substr_size -= 1;
}

void init_nfa(char* regex, size_t regex_size, struct nfa* myNfa) {
	int index = 0;
	char cur;
	struct nfa leftNfa;
	
	// Initialize as empty string NFA
	empty_char_nfa(&leftNfa);
	
	// Iterate through each character in regex
	while (index < regex_size) {
		// Get current character
		cur = regex[index];

		switch(cur) {
		case 'a':
		case 'b':
			{
				struct nfa tempNfa;
				struct nfa tempNfa2;
				// Construct single character NFA
				single_char_nfa(cur, &tempNfa);
				
				// If there's a star after, create before concat
				if (index + 1 < regex_size && regex[index+1] == '*') {
					star_nfa(&tempNfa, &tempNfa2);
					// Concat to leftNfa
					concat_nfa(&leftNfa, &tempNfa2, &tempNfa);
					copy_nfa(&tempNfa, &leftNfa);
					// Increase index by 2 for character and *
					index += 2;
				} else {
					// Concat to leftNfa
					concat_nfa(&leftNfa, &tempNfa, &tempNfa2);
					copy_nfa(&tempNfa2, &leftNfa);
					// Increase index by 1 for the character
					index++;
				}
			}
			break;
		case 'e':
			{
				struct nfa tempNfa;
				struct nfa tempNfa2;
				// Create empty character NFA
				empty_char_nfa(&tempNfa);
				
				// If star resolve
				if (index + 1 < regex_size && regex[index+1] == '*') {
					star_nfa(&tempNfa, &tempNfa2);
					// Concat to leftNfa
					concat_nfa(&leftNfa, &tempNfa2, &tempNfa);
					copy_nfa(&tempNfa, &leftNfa);
					// Increase index by 2 for character and *
					index += 2;
				} else {
					// Concat to leftNfa
					concat_nfa(&leftNfa, &tempNfa, &tempNfa2);
					copy_nfa(&tempNfa2, &leftNfa);
					// Increase index by 1 for e
					index++;
				}
			}
			break;
		case '|':
			{
				size_t substr_size;
				struct nfa tempNfa;
				struct nfa tempNfa2;
				// Find substring to resolve before union
				vertical_extract(regex+index, regex_size-index, &substr_size);
				// Initialize that substring as its own NFA
				init_nfa(regex+index+1, substr_size, &tempNfa);
				// Increase the index by the substring + 1 for '|'
				index += substr_size + 1;
				// Union the leftNfa with the init'ed NFA
				union_nfa(&leftNfa, &tempNfa, &tempNfa2);
				// Copy result to leftNfa
				copy_nfa(&tempNfa2, &leftNfa);
			}
			break;
		case '(':
			{
				size_t substr_size;
				int has_star;
				struct nfa tempNfa;
				struct nfa tempNfa2;
				// Find substring to resolve 
				paran_extract(regex+index, regex_size-index, &substr_size, &has_star);
				// Resolve substring to NFA
				init_nfa(regex+index+1, substr_size, &tempNfa);
				// If star after ()*, perform now
				if(has_star) {
					star_nfa(&tempNfa, &tempNfa2);
					copy_nfa(&tempNfa2, &tempNfa);
					// Increase index for *
					index++;
				}
				// Increase index by substring + 2 for ( and )
				index += substr_size + 2;
				// Concatenate leftNfa with paranthetical
				concat_nfa(&leftNfa, &tempNfa, &tempNfa2);
				// Copy to leftNfa
				copy_nfa(&tempNfa2, &leftNfa);
			}
			break;
		default:
			// Ignore, it's either an unknown entry
			// Or multiple * in a row which is redundant
			index++;
			break;
		}	
	}
	
	// Copy leftNfa to myNfa
	copy_nfa(&leftNfa, myNfa);
}

/**
 * cleanup_nfa_states
 * summary: before creating a DFA, reduce all the states to single character
 * since this makes giving them a separator unnecessary
 */
void cleanup_nfa_states(struct nfa* myNfa) {
	int i;
	
	// Give every state an id equal to i+1
	for (i = 0; i < myNfa->Qcount; i++) {
		int j;
		char c[2] = {i+1, 0};
		char cur[256];
		strcpy(cur, myNfa->Q[i]);
		strcpy(myNfa->Q[i], c);
		
		// Convert all references in transition functions
		for (j = 0; j < myNfa->Tcount; j++) {
			if(!strcmp(cur, myNfa->delta[j].currentState)) {
				strcpy(myNfa->delta[j].currentState, c);
			}
			if(!strcmp(cur, myNfa->delta[j].finalState)) {
				strcpy(myNfa->delta[j].finalState, c);
			}
		}
		
		// Convert all references in Final states
		for (j = 0; j < myNfa->Fcount; j++) {
			if(!strcmp(cur, myNfa->F[j])) {
				strcpy(myNfa->F[j], c);
			}
		}
		
		// Change start state if applies
		if(!strcmp(cur, myNfa->q0)) {
			strcpy(myNfa->q0, c);
		}
	}
}

/**
 * is_final
 * summary: determine if any single-character states from state are in myNfa->F
 */
int is_final(const char *state, struct nfa* myNfa) {
	int i;
	for (i=0; i<myNfa->Fcount; i++) {
		if(strpbrk(myNfa->F[i], state)) {
			return 1;
		}
	}
	return 0;
}

/**
 * insertState
 * summary: insert single-character state c into multi-character DFA state newState
 * It is important that single-character states are sorted in newState
 * to avoid duplicates of the same state with different ordering
 */
void insertState(char c, char* newState) {
	int i = 0;
	char swap;
	char tmp;
	while(newState[i]) {
		if(newState[i] == c) {
			return;
		}
		if(newState[i] > c) {
			break;
		}
		i++;
	}
	swap = newState[i];
	newState[i] = c;
	while(newState[i]) {
		tmp = newState[i+1];
		newState[i+1] = swap;
		swap = tmp;
		i++;
	}
}

/**
 * build_new_states
 * summary: Build new DFA states by finding all 'a' and 'b' transitions from any NFA
 * state listed in thisState.
 */
void build_new_states(const char* thisState, char* newStateA, char* newStateB, struct nfa* myNfa) {
	int i;
	
	// Find all one-step transitions from a state in thisState that take 'a' or 'b'
	for (i=0; i<myNfa->Tcount; i++) {
		if(strpbrk(myNfa->delta[i].currentState, thisState)) {
			int j;
			char newState[2] = {0};
			if (myNfa->delta[i].input == 'a') {
				insertState(myNfa->delta[i].finalState[0], newStateA);
			} else if (myNfa->delta[i].input == 'b') {
				insertState(myNfa->delta[i].finalState[0], newStateB);
			} else {
				continue;
			}
		}
	}

	// Add all empty-string transitions	
	size_t length = strlen(newStateA);
	for (i=0; i<myNfa->Tcount; i++) {
		size_t new_length;
		if(strpbrk(newStateA, myNfa->delta[i].currentState)) {
			if(myNfa->delta[i].input == 'e') {
				insertState(myNfa->delta[i].finalState[0], newStateA);
			}
		}
		new_length = strlen(newStateA);
		if (new_length != length) {
			length = new_length;
			i=0;
		}
	}

	// Add all empty-string transitions
	length = strlen(newStateB);
	for (i=0; i<myNfa->Tcount; i++) {
		size_t new_length;
		if(strpbrk(newStateB, myNfa->delta[i].currentState)) {
			if(myNfa->delta[i].input == 'e') {
				insertState(myNfa->delta[i].finalState[0], newStateB);
			}
		}
		new_length = strlen(newStateB);
		if (new_length != length) {
			length = new_length;
			i=0;
		}
	}
}

/**
 * expand_delta
 * summary: expand the number of transitions allocated
 */
void expand_delta (int* tempTCount, struct dfa* myDfa) {
	int i;
	(*tempTCount) *= 2;
	struct transition* t = malloc(*tempTCount);
	for (i=0; i < myDfa->Tcount; i++) {
		t[i].input = myDfa->delta[i].input;
		strcpy(t[i].currentState, myDfa->delta[i].currentState);
		strcpy(t[i].finalState, myDfa->delta[i].finalState);
	}
	free(myDfa->delta);
	myDfa->delta = t;
}

/**
 * link_new_states
 * summary: Add new states newStateA and newStateB to myDfa
 */
void link_new_states(int index, char* newStateA, char* newStateB, struct dfa* myDfa, struct nfa* myNfa, int* tempTCount) {
	int i;
	int add_state = 1;

	// We are going to add two new transitions, so make sure we have the space.	
	if (myDfa->Tcount + 2 > *tempTCount) {
		expand_delta(tempTCount, myDfa);
	}

	// If 'a' didn't lead to any state, assign to null state
	if (!newStateA[0]) {
		myDfa->delta[myDfa->Tcount].input = 'a';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, "null");
	} else {
		// Search for state if it exists
		int j = 1;
		while(j < myDfa->Qcount) {
			if (!strcmp(newStateA, myDfa->Q[j])) {
				break;
			}
			j++;
		}

		// If it doesn't exist, add it and increment count
		if (j == myDfa->Qcount) {
			strcpy(myDfa->Q[j], newStateA);
			myDfa->Qcount++;
			
			// Add to final states if it includes a final state from NFA
			if (is_final(newStateA, myNfa)) {
				strcpy(myDfa->F[myDfa->Fcount], newStateA);
				myDfa->Fcount++;
			}
		}

		// Add transition function
		myDfa->delta[myDfa->Tcount].input = 'a';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, newStateA);
	}
	
	// Increment Tcount
	myDfa->Tcount++;
		
	// If 'b' didn't lead to any state, assign to null state
	if (!newStateB[0]) {
		myDfa->delta[myDfa->Tcount].input = 'b';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, "null");
	} else {
		// Search for state if it exists
		int j = 1;
		while(j < myDfa->Qcount) {
			if (!strcmp(newStateB, myDfa->Q[j])) {
				break;
			}
			j++;
		}

		// If it doesn't exist, add it and increment count
		if (j == myDfa->Qcount) {
			strcpy(myDfa->Q[j], newStateB);
			myDfa->Qcount++;
			// Add to final states if it includes a final state from NFA
			if (is_final(newStateB, myNfa)) {
				strcpy(myDfa->F[myDfa->Fcount], newStateB);
				myDfa->Fcount++;
			}
		}

		// Add transition function
		myDfa->delta[myDfa->Tcount].input = 'b';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, newStateB);
		
	}

	// Increment Tcount	
	myDfa->Tcount++;
}

void nfa_to_dfa(struct nfa* myNfa, struct dfa* myDfa) {
	int i;
	int tempTCount = 128;
	cleanup_nfa_states(myNfa);
	
	// Add null state at beginning
	myDfa->Qcount = 1;
	myDfa->Fcount = 0;
	
	strcpy(myDfa->Q[0], "null");
	
	// 'a' and 'b' from 'null' go back to 'null'
	myDfa->delta = malloc(sizeof(struct transition) * tempTCount);
	myDfa->delta[0].input = 'a';
	strcpy(myDfa->delta[0].currentState, "null");
	strcpy(myDfa->delta[0].finalState, "null");
	myDfa->delta[1].input = 'b';
	strcpy(myDfa->delta[1].currentState, "null");
	strcpy(myDfa->delta[1].finalState, "null");
	myDfa->Tcount = 2;
	
	// Create start state
	strcpy(myDfa->q0, myNfa->q0);
	
	// Add all empty string reachable states to start state
	for (i=0; i<myNfa->Tcount; i++) {
		if (myNfa->delta[i].input == 'e' && strpbrk(myNfa->delta[i].currentState, myDfa->q0)) {
			int j = 0;
			char swap;
			char tmp;
			while(myDfa->q0[j]) {
				if(myDfa->q0[j] >= myNfa->delta[i].finalState[0]) {
					break;
				}
				j++;
			}
			if (myDfa->q0[j] == myNfa->delta[i].finalState[0]) {
				continue;
			}
			swap = myDfa->q0[j];
			myDfa->q0[j] = myNfa->delta[i].finalState[0];
			while(myDfa->q0[j]) {
				tmp = myDfa->q0[j+1];
				myDfa->q0[j+1] = swap;
				swap = tmp;
				j++;
			}
			i = 0;
		}
	}
	
	// Add start state to Q
	strcpy(myDfa->Q[1], myDfa->q0);
	myDfa->Qcount++;

	// Add start state to final if final	
	if (is_final(myDfa->q0, myNfa)) {
		myDfa->Fcount++;
		strcpy(myDfa->F[0], myDfa->q0);
	}
	
	// Attempt to reach other states
	i = 1;
	while(i < myDfa->Qcount) {
		char thisState[256];
		char newStateA[256] = { 0 };
		char newStateB[256] = { 0 };
		int j;
		
		strcpy(thisState, myDfa->Q[i]);
		build_new_states(thisState, newStateA, newStateB, myNfa);
		link_new_states(i, newStateA, newStateB, myDfa, myNfa, &tempTCount);
		
		i++;
	}
}
