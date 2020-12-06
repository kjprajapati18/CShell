#ifndef CSHELL_H
#define CSHELL_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void loop();
char* read_line();
char** parse(char* inp);
void execute(char** inp);

#endif