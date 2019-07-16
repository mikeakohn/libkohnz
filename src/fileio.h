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

int write16(FILE *out, uint32_t num);
int write32(FILE *out, uint32_t num);
void write_bits(struct _kohnz *kohnz, uint32_t data, int length);
void write_bits_end_block(struct _kohnz *kohnz);

