/**
 *  libkohnz
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2018-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "kohnz.h"

int write16(FILE *out, uint32_t num)
{
  uint8_t data[2];

  data[0] = num;
  data[1] = num >> 8;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

int write32(FILE *out, uint32_t num)
{
  uint8_t data[4];

  data[0] = num;
  data[1] = num >> 8;
  data[2] = num >> 16;
  data[3] = num >> 24;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

void write_bits(struct _kohnz *kohnz, uint32_t data, int length)
{
  struct _bits *bits = &kohnz->bits;

  bits->holding |= data << bits->length;
  bits->length += length;

  while(bits->length >= 8)
  {
    const int byte = bits->holding & 0xff;

    putc(byte, kohnz->out);

    bits->holding >>= 8;
    bits->length -= 8;
  }
}

void write_bits_end_block(struct _kohnz *kohnz)
{
  struct _bits *bits = &kohnz->bits;

  if (bits->length == 0) { return; }

  uint8_t data = bits->holding & ((1 << bits->length) - 1);

  putc(data, kohnz->out);
}

