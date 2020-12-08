#include "cshell.h"
#define BUFSIZE 1024
#define ARGSIZE 5

int numArgs = 0;
int numcmds = 0;
int append = 0;
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
    const char a[3] = ">>";
    const char c[2] = ">";
    const char e[2] = "|";
    int stdOutCopy = dup(1);

    do{
        long size = pathconf(".", _PC_PATH_MAX);
        char* buf = (char*) malloc(size);
        char* cwd = getcwd(buf, size);        
        printf("%s$ ", cwd);
        fflush(0);
        char* line = read_line();
        //printf("line: %s\n", line);
        char** cmds = parse(line, s);
        numcmds = numArgs;

        int i, j;
        for(i =0; i < numcmds; i++){
            char* command = cmds[i];
            char* filepath = strstr(command, ">");

            if(filepath != NULL){
                int file = -1;
                *filepath = '\0'; 
                if(*(filepath+1) == '>'){
                    *(filepath+1) = '\0';
                    filepath = removeSpace(filepath+2);

                    file = open(filepath, O_WRONLY | O_APPEND);
                    if(file < 0){
                        file = open(filepath, O_WRONLY | O_APPEND | O_CREAT, 00777);
                    }
                } else {
                    filepath = removeSpace(filepath+1);
                    remove(filepath);
                    file = open(filepath, O_WRONLY | O_CREAT, 00777);
                }
                
                if(file < 0) continue;
                dup2(file, 1);
                close(file);
            }
            /*parse(command, a);

            if (redirects[1] != NULL){
                printf("yoyo\n");
                int file = open(redirects[1], O_WRONLY | O_APPEND);
                if(file < 0){
                    file = open(redirects[1], O_WRONLY | O_APPEND | O_CREAT, 00777);
                    if(file < 0) continue;
                }
                dup2(file, 1);
            } else{
                free(redirects);
                printf("In here %d\n", i);
                redirects = parse(command, c);
                if(redirects[1] != NULL){
                    remove(redirects[1]);
                    int file = open(redirects[1], O_WRONLY | O_CREAT, 00777);
                    if(file < 0) continue;
                    dup2(file, 1);
                }
            }*/

            char** args = parse(cmds[i], p);
            /*for(j = 0; j < numArgs; j++){
                printf("Arg %d: %s\n", j, args[j]);
            }*/
            //perror("asdfasdfsdfas");
            execute(args);
            free(args);
            dup2(stdOutCopy, 1);
        }
        free(buf);
        free(line);
        
    }while(check);
}

char* removeSpace(char* string){
    int i=0;
    while(isspace(*(string+i))){
        i++;
    }
    return string + i;
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