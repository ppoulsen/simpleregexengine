
#ifndef TRANSITION_H
#define TRANSITION_H
/**
 * Defines a transition function for an NFA or DFA
 */
struct transition {
	char currentState[256];
	char input;
	char finalState[256];
};
#endif /* TRANSITION_H */
