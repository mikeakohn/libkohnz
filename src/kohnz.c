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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crc32.h"
#include "kohnz.h"

static int write16(FILE *out, uint32_t num)
{
  uint8_t data[2];

  data[0] = num >> 8;
  data[1] = num;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

static int write32(FILE *out, uint32_t num)
{
  uint8_t data[4];

  data[0] = num >> 24;
  data[1] = num >> 16;
  data[2] = num >> 8;
  data[3] = num;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

void kohnz_init()
{
  kohnz_crc32_init();
}

struct _kohnz *kohnz_open(const char *filename)
{
  struct _kohnz *kohnz;

  kohnz = (struct _kohnz *)malloc(sizeof(struct _kohnz));

  if (kohnz == NULL) { return NULL; }

  memset(kohnz, 0, sizeof(struct _kohnz));

  kohnz->out = fopen(filename, "wb");

  if (kohnz == NULL)
  {
    free(kohnz);
    return NULL;
  }

  return kohnz;
}

int kohnz_close(struct _kohnz *kohnz)
{
  write32(kohnz->out, kohnz->crc32);
  write32(kohnz->out, kohnz->file_size);

  fclose(kohnz->out);

  free(kohnz);

  return 0;
}

int kohnz_write_00(struct _kohnz *kohnz, const uint8_t *data, int length)
{
  putc(4, kohnz->out);
  write16(kohnz->out, length);
  write16(kohnz->out, length ^ 0xffff);
  fwrite(data, 1, length, kohnz->out);

  kohnz->crc32 = kohnz_crc32(data, length, kohnz->crc32);
  kohnz->file_size += length;

  return 0;
}

