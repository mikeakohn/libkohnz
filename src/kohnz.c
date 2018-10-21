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

  data[0] = num;
  data[1] = num >> 8;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

static int write32(FILE *out, uint32_t num)
{
  uint8_t data[4];

  data[0] = num;
  data[1] = num >> 8;
  data[2] = num >> 16;
  data[3] = num >> 24;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

void kohnz_init()
{
  kohnz_crc32_init();
}

struct _kohnz *kohnz_open(const char *filename, const char *fname, const char *fcomment)
{
  struct _kohnz *kohnz;

  kohnz = (struct _kohnz *)malloc(sizeof(struct _kohnz));

  if (kohnz == NULL) { return NULL; }

  memset(kohnz, 0, sizeof(struct _kohnz));

  kohnz->crc32 = 0xffffffff;

  kohnz->out = fopen(filename, "wb");

  if (kohnz == NULL)
  {
    free(kohnz);
    return NULL;
  }

  uint8_t flags = 0;

  if (fname != NULL && fname[0] != 0) { flags |= 0x08; }
  if (fcomment != NULL && fcomment[0] != 0) { flags |= 0x10; }

  // Magic number
  putc(0x1f, kohnz->out);
  putc(0x8b, kohnz->out);
  // Compression method 8 (DEFLATE)
  putc(0x08, kohnz->out);
  putc(flags, kohnz->out);
  // Timestamp
  write32(kohnz->out, 0);
  // Compression flags
  putc(0x02, kohnz->out);
  // Operating system (3 is Unix)
  putc(0x03, kohnz->out);

  if (fname != NULL && fname[0] != 0)
  {
    fwrite(fname, 1, strlen(fname) + 1, kohnz->out);
  }

  if (fcomment != NULL && fcomment[0] != 0)
  {
    fwrite(fcomment, 1, strlen(fcomment) + 1, kohnz->out);
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
  putc(1, kohnz->out);
  write16(kohnz->out, length);
  write16(kohnz->out, length ^ 0xffff);
  fwrite(data, 1, length, kohnz->out);

  kohnz->crc32 = kohnz_crc32(data, length, kohnz->crc32);
  kohnz->file_size += length;

  return 0;
}

