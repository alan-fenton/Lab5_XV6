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
Part 3: We believe we succeeded at this part, but can't test it without a functional part 4, which we did not achieve.
Part 4: Not functional. Many points along they way included us making what felt like progress, but we ultimately couldn't get past a remap panic and an infinte loop of processes which visually was similar to the effects of a fork bomb.
Part 5:
testcow - Made a short program to show how the number of free pages changes as
          processes are forked and writes are made.
