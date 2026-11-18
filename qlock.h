// kernel/qlock.h
// Queuing lock: like a sleeplock, but waiters sleep in a FIFO queue
// and the lock is handed directly to the process that waited longest.
struct qlock {
  struct spinlock lk;   // protects the fields below
  int locked;           // is the lock held?
  struct proc *owner;   // process holding the lock
  struct proc *head;    // first waiter (FIFO queue)
  struct proc *tail;    // last waiter
  char *name;           // name of lock (for debugging)
};
