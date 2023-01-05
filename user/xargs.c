#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define EOF -1

int getchar(){
    char ch;
    if(read(0,&ch,sizeof(ch)) <= 0)
        return EOF;

    return (int)ch;
}

int readline(char *buf){
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF)
        *buf++ = ch;
    *buf = 0;
    
    if(ch == EOF)
        return EOF;
    return 1;
}

int main(int argc,char *argv[]){
    char buf[512];

    if(argc <= 1){
        fprintf(2,"xarg: no enough arguments\n");
        exit(-1);
    }

    int p = 0,q;
    char *command = argv[1];
    char *args[MAXARG];
    args[p++] = command;
    for(int i = 2;i < argc;i++)
        args[p++] = argv[i]; // arguments from argv
    
    q = p;
    while(readline(buf) != EOF){
        args[q] = malloc(strlen(buf));
        strcpy(args[q++],buf);
    }
    args[q] = 0;

    int status;

    // for(int i =0;args[i] != 0;i++)
    //     printf("debug: %s\n",args[i]);

    if(fork() == 0){ // child
        exec(command,args);
    }else{ // father
        wait(&status);
    }

    for(;p < q;p ++)
        free(args[p]);

    exit(0);
}
