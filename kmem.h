//Wesley created file
#include "spinlock.h"

//These structures have been moved here from kalloc.c so that proc.c can also
//access them.
struct run {
  struct run *next;
  struct spinlock lock;
  int count;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  struct run *multiref;
} kmem;

void pageRefDecCount (struct run * r);
void pageRefIncCount (struct run * r);
