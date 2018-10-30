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

  kohnz_start_fixed_block(kohnz, 1);
  kohnz_write_fixed(kohnz, (const uint8_t *)"MIKEKOHNROCKS", 13);
  //kohnz_write_static(kohnz, (const uint8_t *)"MIKE", 4);
  //kohnz_write_fixed_lz77(kohnz, 8, 4);
  kohnz_write_fixed_lz77(kohnz, 9, 8);
  kohnz_end_fixed_block(kohnz);

  //kohnz_build_crc32(kohnz, (const uint8_t *)"MIKEMIKE", 8);
  kohnz_build_crc32(kohnz, (const uint8_t *)"MIKEKOHNROCKSKOHNROCK", 21);

  kohnz_close(kohnz);

  return 0;
}

