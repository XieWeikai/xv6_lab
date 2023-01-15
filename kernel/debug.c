#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "debug.h"

extern pagetable_t kernel_pagetable;

void debug_info(char *name, struct proc *p){
    printf("----------------%s-----------------\n",name);
    printf("number of free pages:%d\n",num_free_pages());
    printf("kernel:%p  process kernel:%p\n",kernel_pagetable,p->kernel_pgtb);
    printf("check equal: %d\n",pagetable_eq(kernel_pagetable,p->kernel_pgtb));
    printf("-------------------------------------\n");
}
