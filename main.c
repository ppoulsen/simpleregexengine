#include <stdio.h>
#include <stdlib.h>

struct nfa {
};

struct dfa {
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

void init_dfa(char* regex, size_t regex_size, struct dfa* myDfa) {
}

int test_line(char* line, size_t line_size, struct dfa* myDfa) {
	return 0;
}
