all: cshell.c
	gcc -g -o shell cshell.c

clean:
	rm shell