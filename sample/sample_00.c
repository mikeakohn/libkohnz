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

  kohnz_start_uncompressed_block(kohnz);
  kohnz_write_uncompressed(kohnz, (const uint8_t *)"MIKEMIKE", 8);
  kohnz_close(kohnz);

  return 0;
}

