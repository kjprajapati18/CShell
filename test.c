
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
	int n;
	int fd[2];
	fd[0] = 1;
	fd[1] = open("test.txt", O_WRONLY | O_APPEND | O_CREAT, 00777);
	char *data = "hello... this is sample data";
	pipe(fd);
	char buf[1025];
	write(fd[1], data, strlen(data));
	printf("This is a test");
	if ((n = read(fd[0], buf, 1024)) >= 0){
		buf[n] = 0;	// terminate the string
		//buf2[m] = 0;
        printf("read %d bytes from the pipe: \"%s\"\n", n, buf);
       // printf("read %d bytes from the pipe: \"%s\"\n", m, buf2);
	}	
	else
		perror("read");
	close(fd[0]);
	exit(0);
}