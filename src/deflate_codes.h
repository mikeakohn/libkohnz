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

#ifndef _DEFLATE_CODES_H
#define _DEFLATE_CODES_H

extern int deflate_length_codes[29];
extern int deflate_length_extra_bits[29];
extern int deflate_distance_codes[30];
extern int deflate_dist_extra_bits[30];
extern int deflate_reverse[256];

#endif

