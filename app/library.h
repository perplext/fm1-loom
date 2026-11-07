/* SPDX-License-Identifier: MPL-2.0 */
/* LOOM-8's library of beats and chord progressions (library.c) */
#pragma once
#include "seq/pattern.h"
#include "seq/seq.h"

int lib_nbeats(void);
const char *lib_beat_name(int i);               /* (any i: it wraps) */
const char *lib_beat_genre(int i);
int lib_beat_load(tpat_t *tp, int i);           /* the beat onto a kit pattern; returns its length */

int lib_nprogs(void);
const char *lib_prog_name(int i);
const char *lib_prog_genre(int i);
int lib_prog_scale(int i);                      /* its scale, BS_* */
int lib_prog_load(tpat_t *tp, int i, int key);  /* the chords onto the conductor's pattern; returns its scale */
