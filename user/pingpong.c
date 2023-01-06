#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p1[2],p2[2];
  char byte;   // byte

  pipe(p1);  // parent to child
  pipe(p2);  // child to parent

  if(fork() != 0){ // parent
    close(p2[1]);
    close(p1[0]);
    write(p1[1],"0",1); // write a byte
    read(p2[0],&byte,1);      // read a byte
    printf("%d: received pong\n",getpid());
    close(p2[0]);
    close(p1[1]);
    exit(0);
  }else{ // child
    close(p2[0]);
    close(p1[1]);
    read(p1[0],&byte,1);      // read a byte
    printf("%d: received ping\n",getpid());
    write(p2[1],&byte,1); // write a byte
    close(p2[1]);
    close(p1[0]);
    exit(0);
  }
}
