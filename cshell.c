#include "cshell.h"
#define BUFSIZE 1024
#define ARGSIZE 5

int numArgs = 0;
int numcmds = 0;
//TODO LIST:
//Can combine read and parse into one pretty simply. just split up for simplicity atm
//start exec
//parse: first split up into list of each individual command by strtoking with ;, then 
    //we have a loop of executables that we use my execute on
//need to check for piping: > | >>
//need alllll the error checks lmfao


int main(int argc, char** argv){
    loop();
}

void loop(){
    int check = 1;
    const char s[2] = ";";
    const char p[2] = " ";
    do{
        long size = pathconf(".", _PC_PATH_MAX);
        char* buf = (char*) malloc(size);
        char* cwd = getcwd(buf, size);        
        printf("%s$ ", cwd);
        fflush(0);
        char* line = read_line();
        printf("line: %s\n", line);
        char** cmds = parse(line, s);
        numcmds = numArgs;
        int i, j;
        for(i =0; i < numcmds; i++){
            char** args = parse(cmds[i], p);
            for(j = 0; j < numArgs; j++){
                printf("Arg %d: %s\n", j, args[j]);
            }
            execute(args);
            free(args);
        }
        free(buf);
        free(line);
    }while(check);
}

char* read_line(){
    int bufsize = 1024;
    int pos = 0;
    char* buffer = (char*) malloc(bufsize);

    while(1){
        char c;
        read(0, &c, 1);
        if(c == EOF || c == '\n'){
            buffer[pos] = '\0';
            return buffer;
        }
        else{
            buffer[pos] = c;
        }
        pos++;
        if(pos > bufsize){
            bufsize+= BUFSIZE;
            char* newbuf = realloc(buffer, bufsize);
            buffer = newbuf;
        }
    }
    return buffer;
}

char** parse(char* line, const char* c){
    numArgs = 0;
    int argsize = ARGSIZE;
    int pos = 0;
    char** args = (char**) malloc(sizeof(char*)*argsize);
    char* token = strtok(line, c);
    while(token != NULL){
        args[pos] = token;
        numArgs++;
        pos++;
        if(pos >= argsize){
            argsize+= ARGSIZE;
            char** newargs = realloc(args, argsize);
            args = newargs;
        }
        token = strtok(NULL, c);
    }
    args[pos] = NULL;
    return args;
}

//only exec cd and pwd, others are exec'd with bin. 
void execute(char** args){
    if(!strcmp(args[0], "cd")){
        //setup error case
        chdir(args[1]);
        return;
    }
    else if(!strcmp(args[0], "pwd")){
        //setup error case
        long size = pathconf(".", _PC_PATH_MAX);
        char* buf = (char*) malloc(size);
        char* cwd = getcwd(buf, size);        
        printf("%s\n", cwd);
        free(buf);
        return;
    }
    else{
        pid_t pid, wpid;
        int status;
        pid = fork();
        if(pid == 0){
            //printf("NUMARGS: %d\n", numArgs);
            fflush(0);
            execvp(args[0], args);
        }
        else{
            waitpid(-1, &status, WUNTRACED);
        }
    }
    return;
}