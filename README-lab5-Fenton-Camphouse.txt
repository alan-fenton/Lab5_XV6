All files that are normally modified to add a system call were modified to add
getNumFreePages

Part 1:
proc.c - Added the actual code for getNumFreePages
kmem.h - Moved the structures from kalloc.c to here so proc.c can access them
kalloc.c - Moved the structures out of the file

Part 2:
kmem.h - Added necessary things to the kmem structure to keep track of
         references, and put function prototypes for helper functions.
kalloc.c - Modified kfree and kalloc to play nice with reference counting, and
           made helper functions for the reference count.

Part 5:
testcow - Made a short program to show how the number of free pages changes as
          processes are forked and writes are made.
