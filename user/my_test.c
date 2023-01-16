#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    write(1,"Hello, world!\n",14);

    exit(0);
}