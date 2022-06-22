all: main.c
	gcc -Werror -Wall -o main main.c

clean:
	rm -rf main
