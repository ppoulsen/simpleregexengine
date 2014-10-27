1) Which files/functions implement how to convert a regular expression to an NFA
=====
The function declared in nfa.h and defined in nfa.c called init_nfa takes
in a regex and uses helper functions in nfa.c to create an NFA.

2) Which files/functions implement how to convert an NFA to a DFA
=====
The function declared in nfa.h and defined in nfa.c called nfa_to_dfa takes
in an nfa and uses helper functions in nfa.c to create a DFA.

3) Which files/functions implement how to determine whether an input string
is accepted or rejected by a DFA?
=====
The function declared in dfa.h and defined in dfa.c called test_line take in a
line, line_size, and dfa and outputs returns 0 if it doesn't pass and 1 if it
does pass.

4) How to compile files on cse.unl.edu
=====
Unzip files to a directory and run `make`. Then use `./main < inputFile` to
test.

