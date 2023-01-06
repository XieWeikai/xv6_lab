#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc <= 1){
    write(1,"error: no input from user !\n",28);
    exit(-1);
  }
  sleep(atoi(argv[1]));
  exit(0);
}
