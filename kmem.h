//Wesley wrote this file

//These structures have been moved here from kalloc.c so that proc.c can also
//access them.
struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;
