#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dfa.h"

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
