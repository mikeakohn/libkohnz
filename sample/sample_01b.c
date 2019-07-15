/**
 *  libkohnz
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2018-2019 by Michael Kohn
 *
 * An example of creating a .gz with fixed huffman compressed data.
 *
 */

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
  kohnz_write_fixed_lz77(kohnz, 9, 8);
  kohnz_end_fixed_block(kohnz);

  kohnz_build_crc32(kohnz, (const uint8_t *)"MIKEKOHNROCKSKOHNROCK", 21);

  kohnz_close(kohnz);

  return 0;
}

