NAME=decode
TEST_DIR=tests

all:
	gcc -Wall -Werror -Wextra main.c -o $(NAME)

test: $(TEST_DIR)/*
	@for file in $^ ; do \
		./$(NAME) $${file} > .asm; \
		nasm .asm -o .out; \
		(diff .out $${file} && echo [OK] $${file}) || echo [KO] $${file}; \
	done
	@rm .asm .out

