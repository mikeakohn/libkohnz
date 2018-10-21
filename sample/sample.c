#include <stdio.h>
#include <stdlib.h>

#include "kohnz.h"

int main(int argc, char *argv[])
{
  struct _kohnz *kohnz;

  kohnz_init();

  kohnz = kohnz_open("sample.gz");

  if (kohnz == NULL)
  {
    printf("Couldn't open file for writing\n");
    return 0;
  }

  kohnz_write_00(kohnz, (const uint8_t *)"MIKEMIKE", 8);

  kohnz_close(kohnz);

  return 0;
}

