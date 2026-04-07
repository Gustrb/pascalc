.PHONY: all tests

all:
	gcc src/pascalc.c -o dist/pascalc src/io.c src/lexer.c src/ring_buffer.c src/token_pool.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11

tests:
	gcc tests/01-memory-mapping-works.c -o dist/tests/01-memory-mapping-works src/io.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11
	gcc tests/02-ring-buffer-works.c -o dist/tests/02-ring-buffer-works src/ring_buffer.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11
	gcc tests/03-ring-buffer-concurrent.c -o dist/tests/03-ring-buffer-concurrent src/ring_buffer.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11 -pthread
	gcc tests/04-token-pool-works.c -o dist/tests/04-token-pool-works src/token_pool.c src/ring_buffer.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11
	gcc tests/05-token-pool-concurrent.c -o dist/tests/05-token-pool-concurrent src/token_pool.c src/ring_buffer.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11 -pthread
	gcc tests/06-lexer-works.c -o dist/tests/06-lexer-works src/io.c src/token_pool.c src/ring_buffer.c src/lexer.c -Werror -Wall -Wextra -pedantic -fsanitize=address -std=c11 -pthread
