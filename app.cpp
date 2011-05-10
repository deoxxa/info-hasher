#include "info_hasher.h"

int main(int argc, char** argv)
{
  info_hasher ih("database.kch");

  for (int i=1;i<argc;++i)
  {
    ih.check_dir(argv[i]);
  }

  return 0;
}
