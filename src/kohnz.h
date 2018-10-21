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

#ifndef _KOHNZ_H
#define _KOHNZ_H

#include <stdio.h>

struct _kohnz
{
  FILE *out;
};

struct _kohnz *kohnz_open(const char *filename);
int kohnz_close(struct _kohnz *kohnz);

#endif

