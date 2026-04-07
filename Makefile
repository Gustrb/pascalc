all:
	gcc src/pascalc.c -o dist/pascalc src/io.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11
