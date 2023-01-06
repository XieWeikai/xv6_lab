#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 没想到下面的写法真的可以过...
// int is_prime(int n){
//     int p = n/2;
//     for(int i = 2; i <= p;i++){
//         if(n % p == 0)
//             return 0;
//     }
//     return 1;
// }

// int
// main(int argc, char *argv[])
// {
//   for(int i =2;i <= 35;i++){
//     if(is_prime(i))
//         printf("prime %d\n",i);
//   }
//   exit(0);
// }

void worker(int from[]){
    int forked = 0;
    int to[2];
    close(from[1]); // do not write to from
    int p;
    read(from[0],&p,sizeof(p)); // read first p
    printf("prime %d\n",p);
    int t;
    while(read(from[0],&t,sizeof(t) != 0)){
        if(t % p != 0){
            if(!forked){
                pipe(to);
                if(fork() != 0){ // parent
                    close(to[0]);
                }else{ // child
                    worker(to);
                    exit(0);
                }
                forked = 1;
            }
            write(to[1],&t,sizeof(t));
        }
    }
    close(from[0]);
    close(to[1]);
    int status;
    wait(&status);
}

int main(int argc,char *argv[]){
    int p;
    int to[2];
    int status;
    pipe(to);
    
    if(fork() != 0){ // parent
        close(to[0]); // do not read
        for(p = 2;p <= 35;p++)
            write(to[1],&p,sizeof(p));
        close(to[1]);
    }else{ // child
        worker(to);
        exit(0);
    }
    wait(&status);
    exit(0);
}
