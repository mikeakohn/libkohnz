#include <stdio.h>
#include <stdlib.h>

#include "kohnz.h"

int main(int argc, char *argv[])
{
  struct _kohnz *kohnz;
  uint8_t buffer[32];
  int n;

  kohnz_init();

  kohnz = kohnz_open("mikemike.bin.gz", "mikemike.bin", NULL);

  if (kohnz == NULL)
  {
    printf("Couldn't open file for writing\n");
    return 0;
  }

  sprintf((char *)buffer, "I'm a %c%c%ctle teapot.", 0xcc, 0xdd, 0xee);

  kohnz_start_fixed_block(kohnz, 1);
  kohnz_write_fixed(kohnz, buffer, 20);
  kohnz_build_crc32(kohnz, buffer, 20);

  for (n = 0; n < 3400; n++)
  {
    kohnz_write_fixed_lz77(kohnz, 20, 20);
    kohnz_build_crc32(kohnz, buffer, 20);
  }

  kohnz_write_fixed_lz77(kohnz, 2000, 20);
  kohnz_build_crc32(kohnz, buffer, 20);

  kohnz_end_fixed_block(kohnz);
  kohnz_close(kohnz);

  return 0;
}

