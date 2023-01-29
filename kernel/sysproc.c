#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

int user_pointer_ok(uint64 va){
  struct proc *pr = myproc();
  void *pa;
  pte_t *pte;
  if(va >= pr->sz || va >= PLIC)
    return 0;
  pte = walk(pr->pagetable,va,1);
  if(pte == 0)
    return 0;  // walk failed
  if((*pte & PTE_V) != 0 && (*pte & PTE_U) == 0) // guard page, user should not use
    return 0;

  if((*pte & PTE_V) == 0){ // memory not allocated yet
    va = PGROUNDDOWN(va);
    pa = kalloc();
    if(pa == 0)
      return 0; // can not allocate memory
    *pte = PA2PTE(pa) | PTE_U | PTE_R | PTE_W | PTE_V;
    if(mappages(pr->kernel_pgtb,va,PGSIZE,(uint64)pa,PTE_R | PTE_W)){
      uvmunmap(pr->pagetable,va,1,1);
      return 0;
    }
  }
  return 1;
}

uint64
sys_sigalarm(void){
  uint64 fn;
  int ticks;
  struct proc *p = myproc();

  argint(0,&ticks);
  argaddr(1,&fn);
  

  printf("alarm start: fn:%p ticks:%d\n",fn,ticks);

  // stop generating periodic alarm calls
  if(ticks == 0){
    if(fn != 0)
      return -1;
    p->handler = (void (*)()) 0;
    p->tick_cnt = 0;
    p->ticks = 0;
    p->handler_done = 1;
    return 0; // ok
  }

  // ticks != 0, set an alarm handler
  p->ticks = ticks;
  p->tick_cnt = 0;
  p->handler = (void (*)()) fn;
  p->handler_done = 1;

#ifdef DEBUG_TRAP
  printf("alarm done: fn:%p ticks:%d\n",fn,ticks);
#endif

  return 0;
}

uint64
sys_sigreturn(void){
  struct proc *p = myproc();

  p->handler_done = 1;
  p->tick_cnt = 0;

  restore_trapframe(p);

  return 0;
}

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
  if(!user_pointer_ok(p))
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
  if(sz + n >= PLIC)  // user space can not go beyond PLIC
    return -1;
  p->sz += n;

  if(n < 0){
    uvmunmap(p->kernel_pgtb,PGROUNDUP(sz+n),(PGROUNDUP(sz) - PGROUNDUP(sz+n)) / PGSIZE,0);
    uvmdealloc(p->pagetable, sz, sz + n);
  }

  // do not allocate memory eagerly, use lazy allocation strategy
  // if(growproc(n) < 0)
  //   return -1;
  // printf("return sz:%p\n",sz);
  return sz;
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
