all:
	gcc src/pascalc.c -o dist/pascalc -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11
