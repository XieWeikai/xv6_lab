#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
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
