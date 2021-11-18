#include "types.h"
#include "stat.h"
#include "user.h"
#include "fctnl.h"

int
main(int argc, char *argv[])
{
  printf("%d",getNumFreeCells());
  exit();
}
