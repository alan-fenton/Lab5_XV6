//Wesley created file

//These structures have been moved here from kalloc.c so that proc.c can also
//access them.
struct run {
  struct run *next;
  int count;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  uint freepages;
  uint pg_refcount[PHYSTOP/PGSIZE];
} kmem;
void pageRefDecCount (struct run * r);
void pageRefIncCount (struct run * r);
