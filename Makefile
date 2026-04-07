.PHONY: all tests

all:
	gcc src/pascalc.c -o dist/pascalc src/io.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11

tests:
	gcc tests/01-memory-mapping-works.c -o dist/tests/01-memory-mapping-works src/io.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11
