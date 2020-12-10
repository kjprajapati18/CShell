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
    int stdInCopy = dup(0);

   /* int output = open("output.txt", O_CREAT | O_WRONLY, 00777);
    int input = open("input.txt", O_CREAT | O_WRONLY, 00777);
    dup2(input, 0);
    dup2(output, 1);*/

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

        int i, j, k;
        for(i =0; i < numcmds; i++){
            dup2(stdOutCopy, 1);
            dup2(stdInCopy, 0);
            char* command = cmds[i];
            char* filepath = strstr(command, ">");

            char** pipes = parse(command, e);
            int numPipes = numArgs;
            /*int fd[2];
            pipe(fd);
            dup2(fd[0], 0);
            dup2(fd[1], 1);*/
            for(k = 0; k < numPipes-1; k++){
                char** argWpipe = parse(pipes[k], p);
                execute(argWpipe, 1, stdOutCopy, stdInCopy);
                free(argWpipe);
            }
            
            dup2(stdOutCopy, 1);
            //dup2(stdInCopy, 0);
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
            

            char** args = parse(pipes[numPipes-1], p);
            //char** args = parse(command, p);

            execute(args, 0, stdOutCopy, stdInCopy);
            free(args);
            //free(pipes);
            dup2(stdOutCopy, 1);
            dup2(stdInCopy, 0);
            //close(fd[0]);
            //close(fd[1]);
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
void execute(char** args, int pip, int out, int in){

    int fd[2];
    
    if(pip ==1){
        pipe(fd);
    }

    if(!strcmp(args[0], "cd")){
        //setup error case
        chdir(args[1]);
        return;
    }
    else if(!strcmp(args[0], "pwd") && pip == 0){
        //setup error case

        /*if(pip == 1){
            dup2(fd[1], 1);
            dup2(fd[0], 0);
        }*/
        long size = pathconf(".", _PC_PATH_MAX);
        char* buf = (char*) malloc(size);
        char* cwd = getcwd(buf, size);        
        printf("%s\n", cwd);
        // write(1, cwd, size);
        free(buf);
        // if(pip == 1){
        //     close(fd[1]);
        //     close(fd[0]);
        //     dup2(out, 1);
        //     dup2(in, 0);
        // }
        return;
    }
    else if(!strcmp(args[0], "exit")){
        exit(0);
    }
    else{
        pid_t pid, wpid;
        int status;
        pid = fork();
        //printf("%d\n", pid);
        //fflush(0);
        if(pid == 0){
            //printf("NUMARGS: %d\n", numArgs);
            //fflush(0);
            
            /*
            int pid2 = fork();
            
            if(pid2 == 0){
                close(fd[0]);
                char *cmd1[] = { "ls", "-al", 0 };
                execvp(cmd1[0], cmd1);
                exit(0);
            }*/

            if(pip == 1){
                dup2(fd[1], 1);
                close(fd[0]);
            }
            status = execvp(args[0], args);
            //printf("%d\n", status);
        }
        else{
            if(pip ==1){
                dup2(fd[0], 0);
                close(fd[1]);
            }
            waitpid(pid, &status, WUNTRACED);
            
        }
    }
    //perror("Returnign!!");
    return;
}