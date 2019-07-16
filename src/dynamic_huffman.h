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

#ifndef _DYNAMIC_HUFFMAN_H
#define _DYNAMIC_HUFFMAN_H

#include <stdio.h>
#include <stdint.h>

#include "kohnz.h"

int dynamic_huffman_build(
  struct _kohnz *kohnz,
  uint16_t *literals_sorted,
  uint16_t *distances_sorted,
  int literals_count,
  int distances_count);

#endif

