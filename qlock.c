// kernel/qlock.c
// Queuing locks (qlocks), modeled on sleeplock.c.

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "qlock.h"

void
initqlock(struct qlock *q, char *name)
{
  initlock(&q->lk, "qlock");
  q->name = name;
  q->locked = 0;
  q->owner = 0;
  q->head = 0;
  q->tail = 0;
}

void
acquireq(struct qlock *q)
{
  struct proc *p = myproc();

  acquire(&q->lk);
  if(!q->locked){
    // Free: take it immediately.
    q->locked = 1;
    q->owner = p;
  } else {
    // Busy: join the tail of the queue and sleep until handed the lock.
    p->qnext = 0;
    if(q->tail)
      q->tail->qnext = p;
    else
      q->head = p;
    q->tail = p;
    while(q->owner != p){
      // Same pattern as acquiresleep() in this xv6 version:
      sleep_prepare(p);   // sleep on our own proc pointer
      release(&q->lk);
      sleep();
      acquire(&q->lk);
    }
  }
  release(&q->lk);
}

void
releaseq(struct qlock *q)
{
  acquire(&q->lk);
  struct proc *next = q->head;
  if(next){
    // Hand the lock straight to the first waiter (FIFO). It stays locked.
    q->head = next->qnext;
    if(q->head == 0)
      q->tail = 0;
    next->qnext = 0;
    q->owner = next;
    wakeup(next);         // wakes only that one process
  } else {
    q->locked = 0;
    q->owner = 0;
  }
  release(&q->lk);
}

int
holdingq(struct qlock *q)
{
  int r;
  acquire(&q->lk);
  r = q->locked && q->owner == myproc();
  release(&q->lk);
  return r;
}
