//Wesley created file

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
} kmem;

//AJ made this function
void decCount (struct run * r){
  acquire(&(r->lock));
  r->count--;
  release(&(r->lock));
};

//AJ made this function too
void incCount (struct run * r){
  acquire(&(r->lock));
  r->count++;
  release(&(r->lock));
};
