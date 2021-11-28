// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
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
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  if(findMultiref((struct run *)v)){ //This if brought to you by the letters AJ
    cprintf("test 0: %d\n", ((struct run *)v)->count);
    pageRefDecCount((struct run *)v);
    cprintf("test 0.5\n");
    return;
  }

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
    r->count=1; //Brought to you by the letters AJ
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

void pageRefIncCount (struct run * r){
  acquire(&(r->lock));

   //Add r to the multiref list if it is not there
   if(!findMultiref(r)) {
      addMultiref(r);
   }

  r->count++;

  release(&(r->lock));
}

void pageRefDecCount (struct run * r){
  cprintf("\n%d\n", r->lock.locked);
  acquire(&(r->lock));
  r->count--;

  //Remove it from the list if the count is reduced to 1
  if(r->count == 1)
     deleteMultiref(r);

  release(&(r->lock));
}

//These functions were written by Wesley

//This adds the given run instance to the multiref list in kmem
void addMultiref(struct run *r) {
   if(kmem.use_lock)
      acquire(&kmem.lock);

   r->next = kmem.multiref;
   kmem.multiref = r;

   if(kmem.use_lock)
      release(&kmem.lock);
}

//This deletes a given run instance from the multiref list in kmem
//Return 1 if the node was deleted, 0 if it was not found
int deleteMultiref(struct run *r) {
   if(kmem.use_lock)
      acquire(&kmem.lock);


   //Special case for it being the head of the list
   if(kmem.multiref == r) {
      kmem.multiref = r->next;
      if(kmem.use_lock)
         release(&kmem.lock);
      return 1;
   }

   //Loop until we find the predecessor to the target
   struct run *walker = kmem.multiref;
   while(walker->next) {
      //We found it, remove it and return
      if(walker->next == r) {
         walker->next = walker->next->next;
         if(kmem.use_lock)
            release(&kmem.lock);
         return 1;
      }
      walker = walker->next;
   }

   //Never found it, return 0
   if(kmem.use_lock)
      release(&kmem.lock);
   return 0;
}

//This checks to see if the node is in the list
//Return 1 if the node was found, 0 otherwise
int findMultiref(struct run *r) {
   struct run *walker = kmem.multiref;

   //Loop through until we find the target
   while(walker) {
      if(walker == r)
         return 1;

      walker = walker->next;
   }

   return 0;
}

