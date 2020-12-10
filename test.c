
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


#include <stdlib.h>
#include <stdio.h>

void runpipe();

int
main(int argc, char **argv)
{
	int pid, status;
	int fd[2];
	printf("%d, %d\n", fd[0], fd[1]);
	pipe(fd);
	printf("%d, %d\n", fd[0], fd[1]);
	
	switch (pid = fork()) {

	case 0: /* child */
		runpipe(fd);
		exit(0);

	default: /* parent */
		while ((pid = wait(&status)) != -1)
			fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
		break;

	case -1:
		perror("fork");
		exit(1);
	}
	exit(0);
}

char *cmd1[] = { "ls", "-al", 0 };
char *cmd2[] = { "tail", "-3", 0 };

void
runpipe(int pfd[])
{
	int pid;
	int orig = dup(1);
		dup2(pfd[0], 0);

	switch (pid = fork()) {

	case 0: /* child */
	
		printf("yes\n");
		fflush(0);
		close(pfd[1]);	/* the child does not need this end of the pipe */
		printf("no\n");
		execvp(cmd2[0], cmd2);
		perror(cmd2[0]);

	default: /* parent */
		printf("parent\n");
		dup2(pfd[1], 1);
		close(pfd[0]);	/* the parent does not need this end of the pipe */
		execvp(cmd1[0], cmd1);
		perror(cmd1[0]);

	case -1:
		perror("fork");
		exit(1);
	}
}

/*
int
main(int argc, char **argv)
{
	/*int n,m;
	int fd[3];
	char buf[1025];
    char buf2[1025];
	char *data = "hello... this is sample data";

	printf("%d, %d, %d", fd[0], fd[1], fd[2]);
	fflush(0);
	pipe(fd);
    pipe(&fd[1]);
    perror("asef");
	write(fd[1], data, strlen(data));
	if ((n = read(fd[0], buf, 1024)) >= 0 /*&& (m = read(fd[1], buf2, 1024)) >=0 ){
		buf[n] = 0;	/* terminate the string
		buf2[m] = 0;
        printf("read %d bytes from the pipe: \"%s\"\n", n, buf);
        printf("read %d bytes from the pipe: \"%s\"\n", m, buf2);
	}	
	else
		perror("read");
	exit(0);*/

/*
	char asdf[5] = "aadf";

	printf("%s\n", asdf);
	char* a = strtok(asdf, ">>");
	char* b = strtok(NULL, ">");
	printf("%s\n", a);
	if(b==NULL)printf("ULLL");*/
	/*int n;
	int fd[2];
	fd[0] = 1;
	fd[1] = open("test.txt", O_RDWR | O_APPEND | O_CREAT, 00777);
	write(fd[0], "hello\n", 7);

	char *data = "hello... this is sample data\n";

	pipe(fd);
	char buf[1025];
	//write(fd[1], data, strlen(data));
	printf("%s", data);
	/*if ((n = read(fd[0], buf, 1)) >= 0){
		buf[n] = 0;	// terminate the string
		//buf2[m] = 0;
        printf("read %d bytes from the pipe: \"%s\"\n", n, buf);
       // printf("read %d bytes from the pipe: \"%s\"\n", m, buf2);
	}	
	else
		perror("read");
	close(fd[0]);
	exit(0);*/
