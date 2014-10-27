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
	regex_size = strlen(regex);
	init_dfa(regex, regex_size, &myDfa);
	
	while(getline(&line, &line_size, stdin) >= 0) {
		line_size = strlen(line);
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

void paran_extract(char* regex, size_t regex_size, size_t *substr_size, int *has_star) {
	int paren_count = 1;
	*substr_size = 0;
	*has_star = 0;
	if(regex[0] != '(')
		return;
	while(paren_count > 0) {
		(*substr_size)++;
		if (regex[*substr_size] == '(') {
			paren_count++;
		} else if (regex[*substr_size] == ')') {
			paren_count--;
		}
	}
	if (*substr_size + 1 < regex_size) {
		if (regex[*substr_size + 1] == '*') {
			*has_star = 1;
		}
	}
	*substr_size -= 1;
}

void init_nfa(char* regex, size_t regex_size, struct nfa* myNfa) {
	int index = 0;
	char cur;
	struct nfa leftNfa;
	empty_char_nfa(&leftNfa);
	
	while (index < regex_size) {
		cur = regex[index];

		switch(cur) {
		case 'a':
		case 'b':
			{
				struct nfa tempNfa;
				struct nfa tempNfa2;
				single_char_nfa(cur, &tempNfa);
				if (index + 1 < regex_size && regex[index+1] == '*') {
					star_nfa(&tempNfa, &tempNfa2);
					concat_nfa(&leftNfa, &tempNfa2, &tempNfa);
					copy_nfa(&tempNfa, &leftNfa);
					index += 2;
				} else {
					concat_nfa(&leftNfa, &tempNfa, &tempNfa2);
					copy_nfa(&tempNfa2, &leftNfa);
					index++;
				}
			}
			break;
		case 'e':
			{
				struct nfa tempNfa;
				struct nfa tempNfa2;
				empty_char_nfa(&tempNfa);
				if (index + 1 < regex_size && regex[index+1] == '*') {
					star_nfa(&tempNfa, &tempNfa2);
					concat_nfa(&leftNfa, &tempNfa2, &tempNfa);
					copy_nfa(&tempNfa, &leftNfa);
					index += 2;
				} else {
					concat_nfa(&leftNfa, &tempNfa, &tempNfa2);
					copy_nfa(&tempNfa2, &leftNfa);
					index++;
				}
			}
			break;
		case '|':
			{
				struct nfa tempNfa;
				struct nfa tempNfa2;
				if (regex[index+1] ==  '(') {
					size_t substr_size;
					int has_star;
					paran_extract(regex+index+1, regex_size-index-1, &substr_size, &has_star);
					init_nfa(regex+index+2, substr_size, &tempNfa);
					index += substr_size + 2 /* ( and ) */ + 1 /* | */; 
					if (has_star) {
						star_nfa(&tempNfa, &tempNfa2);
						copy_nfa(&tempNfa2, &tempNfa);
					}
				} else if(regex[index+1] == 'e') {
					empty_char_nfa(&tempNfa);
					if ( index + 2 < regex_size && regex[index+2] == '*') {
						star_nfa(&tempNfa, &tempNfa2);
						copy_nfa(&tempNfa2, &tempNfa);
						index++;
					}
					index += 2; /* | and e */
				} else {
					single_char_nfa(regex[index+1], &tempNfa);
					if ( index + 2 < regex_size && regex[index+2] == '*') {
						star_nfa(&tempNfa, &tempNfa2);
						copy_nfa(&tempNfa2, &tempNfa);
						index++;
					}
					index += 2; /* | and a or b */
				}
				union_nfa(&leftNfa, &tempNfa, &tempNfa2);
				copy_nfa(&tempNfa2, &leftNfa);
			}
			break;
		case '(':
			{
				size_t substr_size;
				int has_star;
				struct nfa tempNfa;
				struct nfa tempNfa2;
				paran_extract(regex+index, regex_size-index, &substr_size, &has_star);
				init_nfa(regex+index+1, substr_size, &tempNfa);
				if(has_star) {
					star_nfa(&tempNfa, &tempNfa2);
					copy_nfa(&tempNfa2, &tempNfa);
					index++;
				}
				index += substr_size + 2;
				concat_nfa(&leftNfa, &tempNfa, &tempNfa2);
				copy_nfa(&tempNfa2, &leftNfa);
			}
			break;
		default:
			index++;
			break;
		}	
	}
	
	copy_nfa(&leftNfa, myNfa);
}

void cleanup_nfa_states(struct nfa* myNfa) {
	int i;
	for (i = 0; i < myNfa->Qcount; i++) {
		int j;
		char c[2] = {i+1, 0};
		char cur[256];
		strcpy(cur, myNfa->Q[i]);
		strcpy(myNfa->Q[i], c);
		
		for (j = 0; j < myNfa->Tcount; j++) {
			if(!strcmp(cur, myNfa->delta[j].currentState)) {
				strcpy(myNfa->delta[j].currentState, c);
			}
			if(!strcmp(cur, myNfa->delta[j].finalState)) {
				strcpy(myNfa->delta[j].finalState, c);
			}
		}
		
		for (j = 0; j < myNfa->Fcount; j++) {
			if(!strcmp(cur, myNfa->F[j])) {
				strcpy(myNfa->F[j], c);
			}
		}
		
		if(!strcmp(cur, myNfa->q0)) {
			strcpy(myNfa->q0, c);
		}
	}
}

int is_final(const char *state, struct nfa* myNfa) {
	int i;
	for (i=0; i<myNfa->Fcount; i++) {
		if(strpbrk(myNfa->F[i], state)) {
			return 1;
		}
	}
	return 0;
}

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

void build_new_states(const char* thisState, char* newStateA, char* newStateB, struct nfa* myNfa) {
	int i;
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

void link_new_states(int index, char* newStateA, char* newStateB, struct dfa* myDfa, struct nfa* myNfa, int* tempTCount) {
	int i;
	int add_state = 1;
	
	if (myDfa->Tcount + 2 > *tempTCount) {
		expand_delta(tempTCount, myDfa);
	}

	if (!newStateA[0]) {
		myDfa->delta[myDfa->Tcount].input = 'a';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, "null");
	} else {
		int j = 1;
		while(j < myDfa->Qcount) {
			if (!strcmp(newStateA, myDfa->Q[j])) {
				break;
			}
			j++;
		}

		if (j == myDfa->Qcount) {
			strcpy(myDfa->Q[j], newStateA);
			myDfa->Qcount++;
			if (is_final(newStateA, myNfa)) {
				strcpy(myDfa->F[myDfa->Fcount], newStateA);
				myDfa->Fcount++;
			}
		}

		myDfa->delta[myDfa->Tcount].input = 'a';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, newStateA);
	}
	
	myDfa->Tcount++;
		
	if (!newStateB[0]) {
		myDfa->delta[myDfa->Tcount].input = 'b';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, "null");
	} else {
		int j = 1;
		while(j < myDfa->Qcount) {
			if (!strcmp(newStateB, myDfa->Q[j])) {
				break;
			}
			j++;
		}

		if (j == myDfa->Qcount) {
			strcpy(myDfa->Q[j], newStateB);
			myDfa->Qcount++;
			if (is_final(newStateB, myNfa)) {
				strcpy(myDfa->F[myDfa->Fcount], newStateB);
				myDfa->Fcount++;
			}
		}

		myDfa->delta[myDfa->Tcount].input = 'b';
		strcpy(myDfa->delta[myDfa->Tcount].currentState, myDfa->Q[index]);
		strcpy(myDfa->delta[myDfa->Tcount].finalState, newStateB);
		
	}
	
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
	
	strcpy(myDfa->Q[1], myDfa->q0);
	myDfa->Qcount++;
	
	if (is_final(myDfa->q0, myNfa)) {
		myDfa->Fcount++;
		strcpy(myDfa->F[0], myDfa->q0);
	}
	
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

void init_dfa(char* regex, size_t regex_size, struct dfa* myDfa) {
	struct nfa myNfa;
	init_nfa(regex, regex_size, &myNfa);
	nfa_to_dfa(&myNfa, myDfa);
}

int test_line(char* line, size_t line_size, struct dfa* myDfa) {
	int i;
	char curState[256];
	int is_final = 0;
	strcpy(curState, myDfa->q0);

	for (i = 0; i < line_size; i++) {
		int j;
		char c = line[i];
		for (j=0; j<myDfa->Tcount; j++) {
			if (!strcmp(curState, myDfa->delta[j].currentState) && c == myDfa->delta[j].input) {
				strcpy(curState, myDfa->delta[j].finalState);
				break;
			}
		}
	}
	
	for (i = 0; i < myDfa->Fcount; i++) {
		if (!strcmp(curState, myDfa->F[i])) {
			is_final = 1;
			break;
		}
	}
	return is_final;
}
