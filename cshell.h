#ifndef CSHELL_H
#define CSHELL_H
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

void loop();
char* read_line();
char** parse(char* inp, const char* c);
void execute(char** inp);

char* removeSpace(char* string);

#endif