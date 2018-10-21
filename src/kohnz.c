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

#include "kohnz.h"

struct _kohnz *kohnz_open(const char *filename)
{
  struct _kohnz *kohnz;

  kohnz = (struct _kohnz *)malloc(sizeof(struct _kohnz));

  if (kohnz == NULL) { return NULL; }

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
  free(kohnz);

  return 0;
}

