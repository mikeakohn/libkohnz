#include <stdio.h>
#include <stdlib.h>

#include "kohnz.h"

int main(int argc, char *argv[])
{
  struct _kohnz *kohnz;

  kohnz_init();

  kohnz = kohnz_open("mikemike.txt.gz", "mikemike.txt", NULL);

  if (kohnz == NULL)
  {
    printf("Couldn't open file for writing\n");
    return 0;
  }

  kohnz_start_fixed_block(kohnz);
  kohnz_write_static(kohnz, (const uint8_t *)"MIKE", 4);
  kohnz_write_static(kohnz, (const uint8_t *)"MIKE", 4);
  //kohnz_write_fixed_lz77(kohnz, 4, 4);
  kohnz_end_fixed_block(kohnz);

  kohnz_close(kohnz);

  return 0;
}

