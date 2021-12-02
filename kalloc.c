// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "kmem.h"


void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
    kmem.pg_refcount[V2P(p)/PGSIZE]=0;
    kfree(p);
  }
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  if(getPageRefCount((void*)v) > 1){ //This if brought to you by the letters AJ
    pageRefDecCount((struct run *)v);
cprintf("free test\n");
    return;
  }
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    kmem.pg_refcount[(PHYSTOP-((uint) V2P(r)))/PGSIZE] = 1; //Brought to you by the letters AJ
//cprintf("%x has been kalloc'd\n", V2P(r));
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

void pageRefIncCount (struct run * r){
  //Make sure it's physical memory
  if((uint)r > PHYSTOP)
    r = (struct run*)V2P(r);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  cprintf("%x has been incremented: index = %d, count = %d\n", r, (PHYSTOP-((uint) r))/PGSIZE, kmem.pg_refcount[(PHYSTOP-((uint) r))/PGSIZE]);
  //kmem.pg_refcount[(((uint) r)-EXTMEM)/PGSIZE]++;
  //kmem.pg_refcount[(((uint) r)-PHYSTOP)/PGSIZE]++;
  kmem.pg_refcount[(PHYSTOP-((uint) r))/PGSIZE]++;
  if(kmem.use_lock)
    release(&kmem.lock);
}

void pageRefDecCount (struct run * r){
  //Make sure it's physical memory
  if((uint)r > PHYSTOP)
    r = (struct run*)V2P(r);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  cprintf("%x has been decremented: index = %d, count = %d\n", r, (PHYSTOP-((uint) r))/PGSIZE, kmem.pg_refcount[(PHYSTOP-((uint) r))/PGSIZE]);
  //kmem.pg_refcount[(((uint) r)-EXTMEM)/PGSIZE]--;
  //kmem.pg_refcount[(((uint) r)-PHYSTOP)/PGSIZE]--;
  kmem.pg_refcount[(PHYSTOP-((uint) r))/PGSIZE]--;
  if(kmem.use_lock)
    release(&kmem.lock);
};

int getPageRefCount (void * page){
  //Make sure it's physical memory
  if((uint)page > PHYSTOP)
    page = (void*)V2P(page);

  return kmem.pg_refcount[(PHYSTOP-((uint) page))/PGSIZE];
};
