#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
   int a = 5;
   printf(1, "This program only has the variable a\n");
   printf(1, "The number of available pages to start is %d\n\n", getNumFreePages());
   
   //Fork the first child
   if(!fork()) {
      //Show without write, and then with
      printf(1, "The number of available pages when the first child generates is %d\n\n", getNumFreePages());
      a += 2;
      printf(1, "a was just changed, copy should have just happened\n\n");
      printf(1, "The number of available pages when the first child writes is %d\n\n", getNumFreePages());

      //Make a grandchild
      if(!fork()) {
         //Show without write, and then with
         printf(1, "The number of available pages when the first grandchild generates is %d\n\n", getNumFreePages());
         a += 2;
         printf(1, "a was just changed, copy should have just happened\n\n");
         printf(1, "The number of available pages when the first grandchild writes is %d\n\n", getNumFreePages());
      }
      else
         wait();
   }
   else
      wait();

   printf(1, "The number of available pages at the end is %d\n\n", getNumFreePages());
   exit();
}
