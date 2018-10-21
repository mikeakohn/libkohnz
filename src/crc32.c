/**
 *  libkohnz
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2018 by Michael Kohn
 *
 */

#include "crc32.h"

static uint32_t crc_table[256];

int kohnz_crc32_init()
{
  uint32_t c;
  uint32_t n;
  int bit;

  for (n = 0; n < 256; n++)
  {
    c = n;

    for (bit = 0; bit < 8; bit++)
    {
      if (c & 1)
      {
        c = 0xedb88320 ^ (c >> 1);
      }
        else
      {
        c = c >> 1;
      }
    }

    crc_table[n] = c;
  }

  return 0;
}

uint32_t kohnz_crc32(const uint8_t *buffer, int len, uint32_t crc)
{
  int n;

  for (n = 0; n < len; n++)
  {
    crc = crc_table[(crc ^ buffer[n]) & 0xff] ^ (crc >> 8);
  }

  return crc;
}

