#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int sz;
  int n;
  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;
  sz = p->sz;
  p->sz += n;
  
  if(n < 0)
    uvmdealloc(p->pagetable, sz, sz + n);

  // do not allocate memory eagerly
  // if(growproc(n) < 0)
  //   return -1;
  return sz;
}

int user_pointer_ok(uint64 va){
  struct proc *pr = myproc();
  void *pa;
  pte_t *pte;
  if(va >= pr->sz)
    return -1;
  pte = walk(pr->pagetable,va,1);
  if(pte == 0)
    return -2;  // walk failed
  if((*pte & PTE_V) != 0 && (*pte & PTE_U) == 0) // guard page, user should not use
    return -3;

  if((*pte & PTE_V) == 0){ // memory not allocated yet
    va = PGROUNDDOWN(va);
    pa = kalloc();
    if(pa == 0)
      return 0; // can not allocate memory
    *pte = PA2PTE(pa) | PTE_U | PTE_R | PTE_W | PTE_V;
  }
  return 1;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  backtrace(); // for backtrace task

  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
