#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "pinfo.h"
#define K 1000000
extern struct proc proc[NPROC];


uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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
uint64
sys_sysinfo(int)
{
  int param;
  argint(0, &param);
  return sysinfo(param);
}

uint64
sys_procinfo(void)
{
  uint64 uaddr; // user pointer 

  argaddr(0, &uaddr);  //validate pointer argument 
  if (uaddr == 0)
    return -1; 

  struct pinfo k; 
  //fail to copy user space 
  if (copyout(myproc()->pagetable, uaddr, (char *)&k, sizeof(k)) < 0)
    return -1; 
  return procinfo((struct pinfo *)uaddr); 
}



uint64
sys_sched_statistics(void)
{
  struct proc *p;
//loops through all processes in the system
  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    //if process not in use then it prints the information about the process
    if (p->state != UNUSED) {
      printf("%d(%s): tickets: %d, ticks: %d\n",
             p->pid, p->name, p->tickets, p->sched_ticks);
    }
    //release the lock
    release(&p->lock);
  }
  return 0;
}

uint64
sys_sched_tickets(void)
{
  //lets take in the integer ticketing
  int ticketing;
  argint(0, &ticketing);
  
  //if it is too small then lets set it to 1
  if (ticketing < 1)
    ticketing = 1; 
  //if its too large then set it to 10000
  if (ticketing > 10000)
    ticketing = 10000; 
  
  struct proc *p = myproc();
  acquire(&p->lock);
  //assigning the tickets and the stride value here for scheduling
  p->tickets = ticketing;
  p->stride = K / p->tickets;
  if (p->stride < 1)  p->stride = 1;
  release(&p->lock);
  return 0;
}

uint64
sys_clone(void)
{
  uint64 stack;
  argaddr(0, &stack);   
  return clone(stack);
}