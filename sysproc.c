#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

extern int syscount[];	 // array mapping systemcall to a count indicationcation a given system call has been called
extern int numsyscalls;  // The number of system calls defined

// return the counter of how many times
// a given syscall has been called
int
sys_getsyscount(void)
{
  int num;
  if (argint(0, &num) < 0)
  {
    return -1;
  }
  if (num < 0 || num >= numsyscalls)
  {
    return -1;
  }
  return syscount[num];
}

int 
sys_gettree(void) 
{
return 0;
}

int
sys_gethistory(void)
{
return 0;
}

int
sys_clone(void)
{
  int fcn, arg1, arg2, stack;
  if(argint(0, &fcn)<0 || argint(1, &arg1)<0 || argint(2, &arg2)<0 || argint(3, &stack)<0)
    return -1;
  return clone((void *)fcn, (void *)arg1, (void *)arg2, (void *)stack);
}

int
sys_join(void)
{
  void **stack;
  int stackArg;
  stackArg = argint(0, &stackArg);
  stack = (void**) stackArg;
  return join(stack);
}

int
sys_getpinfo(void)
{
  struct pstat *ps;
  
  if(argptr(0, (void*)&ps, sizeof(*ps)) < 0)
    return -1;
  
  return getpinfo(ps);
}
