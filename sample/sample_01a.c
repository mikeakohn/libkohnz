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

  // Start a compressed block with final block flag set to 1.
  // Write straight ASCII "MIKE" to the file.
  // Tell compressor the next 4 bytes will be a distance of 4 bytes backward
  // and will be a length of 4 bytes (repeating "MIKE" again).
  // End the compressed block.
  kohnz_start_fixed_block(kohnz, 1);
  kohnz_write_fixed(kohnz, (const uint8_t *)"MIKE", 4);
  kohnz_write_fixed_lz77(kohnz, 4, 4);
  kohnz_end_fixed_block(kohnz);

  // NOTE: The crc can be computed at any time and could be broken up
  //       into multiple calls.  So kohnz_build_crc32 could be called
  //       twice with a "MIKE", 4 being passed to it.
  kohnz_build_crc32(kohnz, (const uint8_t *)"MIKEMIKE", 8);

  kohnz_close(kohnz);

  return 0;
}

