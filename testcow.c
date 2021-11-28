#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  printf(1, "%d\n", getNumFreePages());

   if(!fork())
      printf(1, "%d\n", getNumFreePages());
   else
      wait();
   exit();
}
