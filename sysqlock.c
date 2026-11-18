// kernel/sysqlock.c
// Test system call for qlocks: a shared counter (the "resource")
// protected by a qlock. Each call does n locked increments.

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "qlock.h"

struct qlock testlock;     // the qlock
int shared_counter = 0;    // the resource it protects

void
qtestinit(void)
{
  initqlock(&testlock, "testlock");
}

uint64
sys_qtest(void)
{
  int n;
  argint(0, &n);

  for(int i = 0; i < n; i++){
    acquireq(&testlock);
    int pid = myproc()->pid;
    printk("pid %d ACQUIRED qlock\n", pid);

    int tmp = shared_counter;   // read
    yield();                    // give up the CPU while holding the lock
    shared_counter = tmp + 1;   // write (an update would get lost without the lock)

    printk("pid %d RELEASED qlock, counter = %d\n", pid, shared_counter);
    releaseq(&testlock);
  }
  return shared_counter;
}
