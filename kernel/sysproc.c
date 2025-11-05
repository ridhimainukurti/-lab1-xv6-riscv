#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "pinfo.h"
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

  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->state != UNUSED) {
      printf("%d(%s): tickets: %d, ticks: %d\n",
             p->pid, p->name, p->tickets, p->sched_ticks);
    }
    release(&p->lock);
  }
  return 0;
}

uint64
sys_sched_tickets(void)
{
  int t;
  argint(0, &t);
  struct proc *p = myproc();

  if (t <= 0)
    return 0;
  if (t > 10000)
    return 0;
  acquire(&p->lock);
  p->tickets = t;   // assumes struct proc has 'int tickets;'
  release(&p->lock);
  return 0;
}