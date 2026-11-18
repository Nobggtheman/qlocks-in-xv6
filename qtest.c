// user/qtest.c
// Several processes increment a shared kernel counter that is
// protected by a qlock. Final value must equal NCHILD * ITERS.
#include "kernel/types.h"
#include "user/user.h"

#define NCHILD 3
#define ITERS  3

int
main(int argc, char *argv[])
{
  printf("qtest: %d processes x %d increments\n", NCHILD, ITERS);

  for(int i = 0; i < NCHILD; i++){
    if(fork() == 0){
      qtest(ITERS);
      exit(0);
    }
  }
  for(int i = 0; i < NCHILD; i++)
    wait(0);

  int final = qtest(0);   // 0 iterations: just read the counter
  printf("final counter = %d (expected %d) %s\n",
         final, NCHILD * ITERS,
         final == NCHILD * ITERS ? "PASS" : "FAIL");
  exit(0);
}
