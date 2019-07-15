/**
 *  libkohnz
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2018-2019 by Michael Kohn
 *
 * An example of creating a .gz with uncompressed data.
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

  kohnz_start_uncompressed_block(kohnz);
  kohnz_write_uncompressed(kohnz, (const uint8_t *)"MIKEMIKE", 8);
  kohnz_close(kohnz);

  return 0;
}

