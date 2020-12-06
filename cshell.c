#include "cshell.h"

int main(int argc, char** argv){
    loop();
}

void loop(){
    do{
        char* cwd = getcwd();
        printf("%s $", cwd);
    }while(1)
}