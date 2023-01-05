#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path,char *filename){
    int fd;
    struct stat st;
    struct dirent de;
    char buff[512];
    char *p;

    strcpy(buff,path);
    p = buff + strlen(buff);
    *p ++ = '/';

    if((fd = open(path,0)) < 0){
        fprintf(2,"find: cannot open file:%s\n",path);
        return;
    }

    if(fstat(fd,&st) < 0){
        fprintf(2,"find: cannot stat\n");
        close(fd);
        return;
    }
    
    if(st.type != T_DIR){
        fprintf(2,"find: path:%s is not a directory\n",path);
        close(fd);
        return;
    }

    while(read(fd,&de,sizeof(de)) == sizeof(de)){
        if(de.inum == 0 || strcmp(de.name,".") == 0 || strcmp(de.name,"..") == 0)
            continue;
        strcpy(p,de.name);
        stat(buff,&st);
        switch (st.type)
        {
        case T_FILE:
            if(strcmp(de.name,filename) == 0)
                printf("%s\n",buff);
            break;
        case T_DIR:
            find(buff,filename); // find recursively
            break;
        default:
            break;
        }
    }

}

int main(int argc,char *argv[]){
    if(argc <= 2){
        fprintf(2,"find: no enough argument\n");
        exit(-1);
    }
    find(argv[1],argv[2]);

    exit(0);
}
