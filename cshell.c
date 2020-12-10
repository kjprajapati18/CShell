#include "cshell.h"
#define BUFSIZE 1028
#define ARGSIZE 5

int numArgs = 0;
int numcmds = 0;
int append = 0;

pid_t pid = -1;
int killed = 0;

char* buf;

//TODO LIST:
//ls | od -c | od -c | od -c | od -c | od -c | wc -c
//The only type of commmand that seems to not be working (less od's work && more pipes work)
//      Ex: ls | od -c | od -c | od -c | od -c | wc -c | od -c| od -c| od -c| wc -c | wc -c
//      This works^


//Semi-colons are working. Int Handler is working. Redirection working
//Exit appears to be working but havent tested rigorously
//      Only tested exit after doing a bunch of commands with redirect/pipe/interrupt
//      Exit stalls/double prints with the first command in TODO. Prolly not be an issue with exit tho

//AND YES, I MADE SURE THEY WORK WITH THE ./shell SCRIPT. NOT OS BASH


void sigHand(){
    if (pid != -1) kill(pid, SIGKILL);
    killed = 1;
    long size = pathconf(".", _PC_PATH_MAX);
    //char* buf = (char*) malloc(size);
    char* cwd = getcwd(buf, size);        
    printf("\n%s$ ", cwd);
    fflush(0);
    //free(buf);
}


int main(int argc, char** argv){
    signal(SIGINT, sigHand);
    loop();
}

void loop(){
    int check = 1;
    int file = -1;
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
    long size;
    //char* buf;
    char* cwd;
    do{
        if (killed == 0){
            size = pathconf(".", _PC_PATH_MAX);
            buf = (char*) malloc(size);
            cwd = getcwd(buf, size);        
            printf("%s$ ", cwd);
            fflush(0);
        }else {
            killed = 0;
        }
        char* line = read_line();
        if(line == NULL) continue;
        killed = 0;
        //printf("line: %s\n", line);
        char** cmds = parse(line, s);
        numcmds = numArgs;

        int i, j, k;
        for(i =0; i < numcmds; i++){
            //dup2(stdOutCopy, 1);
            //dup2(stdInCopy, 0);
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
                if(killed == 1) break;
            }
            if(killed == 1) break;
            dup2(stdOutCopy, 1);
            //dup2(stdInCopy, 0);
            if(filepath != NULL){
                file = -1;
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
            }
            

            char** args = parse(pipes[numPipes-1], p);
            //char** args = parse(command, p);

            execute(args, 0, stdOutCopy, stdInCopy);
            free(args);
            free(pipes);
            if(file > 0){
                close(file);
                file = -1;
            }
            dup2(stdOutCopy, 1);
            dup2(stdInCopy, 0);
            //close(fd[0]);
            //close(fd[1]);
        }
        //free(buf);
        free(line);
        free(cmds);
        
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
            //char* newbuf = realloc(buffer, bufsize);
            char* newbuf = malloc(bufsize);
            if(newbuf == NULL) printf("New buffer is null\n");
            memcpy(newbuf, buffer, pos);
            free(buffer);
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
            //char** newargs = realloc(args, argsize);
            char** newargs = malloc(argsize * sizeof(char*));
            if(newargs == NULL) printf("New args is null\n");
            memcpy(newargs, args, pos * sizeof(char*));
            free(args);
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
        _exit(0);
    }
    else{
        pid_t wpid;
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
            if(killed == 1) pid = -1;
        }
    }
    //perror("Returnign!!");
    return;
}