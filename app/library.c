/* SPDX-License-Identifier: MPL-2.0 */
/* LOOM-8's library: ready-made beats for the kit and chord progressions for the conductor, by genre, to turn
 * through and load with one press (the Steps list: BEAT on a kit track, PROGRESSION on the conductor; the host:
 * beat / prog). A beat is 16 or 32 steps at a sixteenth each as bit masks a sound (packed from strings by hand: see
 * beat_t), the pattern's length set to fit. A progression is a scale and up to eight bars of chords, each a root (semitones above the key) and a
 * shape; it goes on the conductor's pattern a bar a beat-step (its scale 4: a step a beat), one chord a bar, so
 * a track that follows the conductor moves with it. Both only load; the player edits them after as any pattern. */
#include "library.h"

/* (no libc on the device: memset is the compiler's own call, a string compare is ours) */
int lib_streq(const char *a, const char *b)
{
    while (*a && *a == *b)
        a++, b++;
    return *a == *b;
}

/* ---- beats: the sounds in the default kit's order: kick, snare, hat, open, clap, ltom, htom, conga, shaker */
enum { LB_KICK, LB_SNARE, LB_HAT, LB_OPEN, LB_CLAP, LB_LTOM, LB_HTOM, LB_CONGA, LB_SHAKER, LB_N };
/* a beat's lanes packed in LANES (a byte stream, for the flash): a lane is a byte, its sound in the low nibble and
 * flags above (0x10 hard hits follow, 0x20 ghosts, 0x40 soft, 0x80 the beat's last lane), then its hits as a bit
 * mask (bit k = step k), len / 8 bytes little-endian, then each flagged mask the same width: hard hits play at 127,
 * ghosts at 45, soft at 80, the rest at 100. The source strings ('.' 'x' 'X' 'g' 'o', a sixteenth a step) pack to
 * these by tools/beats.py. The genres sit in LB_GENRE (a byte an entry, not a pointer) */
typedef struct {
    const char *name;
    uint16_t off;                   /* into LANES */
    uint8_t genre, len;
} beat_t;
static const char *const LB_GENRE[] = {"dungeon", "darkwave", "blackgaze", "dubstep", "drumstep", "trap", "dnb", "house", "techno", "hiphop", "breaks"};
static const uint8_t LANES[] = {
    0, 1, 1, 1, 0, 1, 133, 68, 68, 0, 1, 0, 129, 16, 0, 0, 73, 73, 1, 16, 16, 133, 73, 73,
    0, 17, 17, 1, 0, 1, 130, 68, 68, 0, 5, 5, 1, 16, 16, 5, 5, 5, 134, 16, 16, 0, 17, 17,
    1, 16, 16, 130, 85, 85, 0, 1, 9, 17, 16, 16, 16, 16, 130, 255, 255, 0, 65, 1, 17, 16, 16, 16,
    16, 2, 85, 85, 133, 0, 192, 0, 17, 17, 1, 16, 16, 130, 68, 68, 0, 85, 85, 129, 170, 170, 0, 255,
    255, 129, 255, 255, 0, 85, 85, 129, 68, 68, 0, 65, 1, 17, 0, 1, 0, 1, 130, 85, 85, 0, 1, 0,
    1, 1, 17, 0, 0, 1, 0, 0, 0, 1, 0, 2, 17, 17, 17, 17, 131, 0, 0, 0, 16, 0, 17, 17,
    129, 68, 68, 0, 1, 1, 1, 16, 16, 130, 85, 85, 0, 1, 4, 17, 0, 1, 0, 1, 130, 85, 85, 0,
    1, 5, 49, 0, 65, 0, 1, 0, 64, 130, 93, 93, 0, 129, 4, 17, 0, 1, 0, 1, 130, 68, 68, 0,
    1, 4, 177, 0, 129, 0, 1, 0, 128, 0, 9, 36, 17, 0, 1, 0, 1, 130, 255, 255, 0, 65, 4, 17,
    0, 1, 0, 1, 130, 213, 213, 0, 1, 4, 145, 16, 16, 16, 16, 0, 5, 12, 177, 144, 18, 16, 16, 128,
    2, 0, 1, 20, 177, 144, 20, 16, 16, 128, 4, 0, 17, 17, 129, 16, 16, 0, 17, 17, 2, 85, 85, 131,
    68, 68, 0, 17, 17, 2, 68, 68, 132, 16, 16, 0, 65, 1, 145, 16, 16, 16, 16, 0, 1, 20, 49, 80,
    16, 16, 16, 64, 0, 130, 85, 85,
};
static const beat_t BEATS[] = {
    {"crypt march", 0, 0, 16},
    {"torchlight", 9, 0, 16},
    {"war drums", 15, 0, 16},
    {"procession", 24, 0, 16},
    {"tambour", 33, 0, 16},
    {"darkwave", 45, 1, 16},
    {"coldwave", 54, 1, 16},
    {"gated snare", 65, 1, 16},
    {"minimal wave", 79, 1, 16},
    {"blast", 88, 2, 16},
    {"hyperblast", 94, 2, 16},
    {"d-beat", 100, 2, 16},
    {"doom", 106, 2, 16},
    {"doom slow", 117, 2, 32},
    {"skank", 141, 2, 16},
    {"tremolo roll", 147, 2, 16},
    {"dubstep", 156, 3, 16},
    {"dubstep 2", 167, 3, 16},
    {"halftime", 180, 3, 16},
    {"drumstep", 191, 4, 16},
    {"drumstep 2", 201, 4, 16},
    {"trap", 212, 5, 16},
    {"two step", 223, 6, 16},
    {"amen", 231, 6, 16},
    {"jungle", 241, 6, 16},
    {"four floor", 251, 7, 16},
    {"techno", 257, 8, 16},
    {"dub techno", 266, 8, 16},
    {"boom bap", 275, 9, 16},
    {"breakbeat", 283, 10, 16},
};
#define NBEATS ((int)(sizeof BEATS / sizeof BEATS[0]))

int lib_nbeats(void) { return NBEATS; }
const char *lib_beat_name(int i) { return BEATS[((i % NBEATS) + NBEATS) % NBEATS].name; }
const char *lib_beat_genre(int i) { return LB_GENRE[BEATS[((i % NBEATS) + NBEATS) % NBEATS].genre]; }

/* the beat into a kit track's pattern: its steps replaced, its length set, a sixteenth a step. Returns the length */
int lib_beat_load(tpat_t *tp, int i)
{
    const beat_t *b = &BEATS[((i % NBEATS) + NBEATS) % NBEATS];
    const uint8_t *p = LANES + b->off;
    int k, s, w = b->len / 8, last = 0;
    for (k = 0; k < NSTEP; k++) {
        memset(&tp->st[k], 0, sizeof tp->st[k]);
        tp->st[k].vel = 100;
    }
    while (!last) {
        uint32_t m[4] = {0, 0, 0, 0};              /* hits, hard, ghost, soft */
        int f = *p++, sound = f & 15;
        last = f & 0x80;
        for (k = 0; k < 4; k++)
            if (k == 0 || (f & (0x10 << (k - 1))))
                for (s = 0; s < w; s++)
                    m[k] |= (uint32_t)*p++ << (8 * s);
        for (s = 0; s < b->len && s < 32; s++) {
            uint32_t bit = 1u << s;
            step_t *st = &tp->st[s];
            int vel;
            if (!(m[0] & bit))
                continue;
            vel = m[1] & bit ? 127 : m[2] & bit ? 45 : m[3] & bit ? 80 : 100;
            kit_add(st, sound + 1);
            if (vel > st->vel || kit_mask(st) == 1u << sound)   /* the loudest sound sets the step's velocity */
                st->vel = (uint8_t)vel;
        }
    }
    tp->len = b->len;
    tp->scale = TS_1;
    tp->lstart = tp->lend = 0;
    return tp->len;
}

/* ---- progressions: a scale, a key offset the player keeps (the conductor's key), chords a bar */
enum { CH_TRIAD, CH_POWER, CH_SUS2, CH_SUS4, CH_MIN7, CH_MAJ7, CH_DOM7, CH_ADD9, CH_MIN, CH_MAJ, CH_DIM };
/* a chord a byte: its root (semitones above the key, the low nibble) and its shape above */
typedef struct {
    const char *name;
    uint8_t genre;                  /* into LP_GENRE */
    uint8_t scale;                  /* BS_* */
    uint8_t nbars;
    uint8_t c[8];
} prog_t;
static const char *const LP_GENRE[] = {"dungeon", "darkwave", "blackgaze", "dubstep", "dnb", "pop", "jazz", "house"};
enum { G_DUNGEON, G_DARKWAVE, G_BLACKGAZE, G_DUBSTEP, G_DNB, G_POP, G_JAZZ, G_HOUSE };
#define C(r, sh) ((r) | ((sh) << 4))
#define T(r) C(r, CH_TRIAD)
static const prog_t PROGS[] = {
    /* dungeon synth: modal, slow */
    {"i VI III VII", G_DUNGEON, BS_MINOR, 4, {T(0), T(8), T(3), T(10)}},
    {"i VII VI VII", G_DUNGEON, BS_MINOR, 4, {T(0), T(10), T(8), T(10)}},
    {"i iv i v", G_DUNGEON, BS_MINOR, 4, {T(0), T(5), T(0), T(7)}},
    {"i bII i v", G_DUNGEON, BS_PHRYGIAN, 4, {T(0), T(1), T(0), C(7, CH_POWER)}},
    {"i VI VII i", G_DUNGEON, BS_MINOR, 4, {T(0), T(8), T(10), T(0)}},
    {"dorian lift", G_DUNGEON, BS_DORIAN, 4, {T(0), T(5), T(10), T(0)}},
    {"harm minor", G_DUNGEON, BS_HARM_MINOR, 4, {T(0), T(8), T(7), T(0)}},
    {"i VI i VII", G_DUNGEON, BS_MINOR, 4, {T(0), T(8), T(0), T(10)}},
    /* darkwave: minor pop shapes */
    {"vi IV I V", G_DARKWAVE, BS_MINOR, 4, {T(0), T(8), T(3), T(10)}},
    {"i VII VI V", G_DARKWAVE, BS_HARM_MINOR, 4, {T(0), T(10), T(8), T(7)}},
    {"i III VII iv", G_DARKWAVE, BS_MINOR, 4, {T(0), T(3), T(10), T(5)}},
    {"i v VI III", G_DARKWAVE, BS_MINOR, 4, {T(0), T(7), T(8), T(3)}},
    {"sus two", G_DARKWAVE, BS_MINOR, 4, {C(0, CH_SUS2), C(8, CH_SUS2), C(10, CH_SUS2), C(5, CH_SUS2)}},
    /* blackgaze / doom: power chords, phrygian, tritones */
    {"phrygian 5ths", G_BLACKGAZE, BS_PHRYGIAN, 4, {C(0, CH_POWER), C(1, CH_POWER), C(10, CH_POWER), C(8, CH_POWER)}},
    {"i VI III VII 5", G_BLACKGAZE, BS_MINOR, 4, {C(0, CH_POWER), C(8, CH_POWER), C(3, CH_POWER), C(10, CH_POWER)}},
    {"tritone drop", G_BLACKGAZE, BS_MINOR, 4, {C(0, CH_POWER), C(6, CH_POWER), C(5, CH_POWER), C(0, CH_POWER)}},
    {"doom descent", G_BLACKGAZE, BS_MINOR, 4, {C(0, CH_POWER), C(10, CH_POWER), C(8, CH_POWER), C(7, CH_POWER)}},
    {"shoegaze", G_BLACKGAZE, BS_MAJOR, 4, {C(0, CH_ADD9), C(9, CH_MIN7), C(5, CH_MAJ7), C(7, CH_SUS4)}},
    {"dim rise", G_BLACKGAZE, BS_HARM_MINOR, 4, {C(0, CH_MIN), C(11, CH_DIM), C(0, CH_MIN), C(8, CH_MAJ)}},
    /* dubstep / drumstep / bass: minor, dark */
    {"i VI VII", G_DUBSTEP, BS_MINOR, 4, {T(0), T(8), T(10), T(0)}},
    {"i iv VI v", G_DUBSTEP, BS_MINOR, 4, {T(0), T(5), T(8), T(7)}},
    {"minor 7ths", G_DUBSTEP, BS_MINOR, 4, {C(0, CH_MIN7), C(8, CH_MAJ7), C(3, CH_MAJ7), C(10, CH_DOM7)}},
    {"i bII", G_DUBSTEP, BS_PHRYGIAN, 2, {C(0, CH_POWER), C(1, CH_POWER)}},
    {"liquid", G_DNB, BS_DORIAN, 4, {C(0, CH_MIN7), C(5, CH_DOM7), C(10, CH_MAJ7), C(3, CH_MAJ7)}},
    /* house / general */
    {"I V vi IV", G_POP, BS_MAJOR, 4, {T(0), T(7), T(9), T(5)}},
    {"ii V I", G_JAZZ, BS_MAJOR, 4, {C(2, CH_MIN7), C(7, CH_DOM7), C(0, CH_MAJ7), C(0, CH_MAJ7)}},
    {"deep house", G_HOUSE, BS_DORIAN, 4, {C(0, CH_MIN7), C(0, CH_MIN7), C(5, CH_MIN7), C(10, CH_MAJ7)}},
};
#define NPROGS ((int)(sizeof PROGS / sizeof PROGS[0]))

int lib_nprogs(void) { return NPROGS; }
const char *lib_prog_name(int i) { return PROGS[((i % NPROGS) + NPROGS) % NPROGS].name; }
const char *lib_prog_genre(int i) { return LP_GENRE[PROGS[((i % NPROGS) + NPROGS) % NPROGS].genre]; }
int lib_prog_scale(int i) { return PROGS[((i % NPROGS) + NPROGS) % NPROGS].scale; }

/* a chord's notes above a root (MIDI), the shape's intervals; a triad is one note (the conductor makes its diatonic
 * triad in its scale) */
static int chord_notes(int root, int shape, uint8_t *nt)
{
    static const int8_t IV[][4] = {
        {0, -1, -1, -1},        /* triad: the root alone */
        {0, 7, 12, -1},         /* power */
        {0, 2, 7, -1},          /* sus2 */
        {0, 5, 7, -1},          /* sus4 */
        {0, 3, 7, 10},          /* min7 */
        {0, 4, 7, 11},          /* maj7 */
        {0, 4, 7, 10},          /* dom7 */
        {0, 4, 7, 14},          /* add9 */
        {0, 3, 7, -1},          /* minor */
        {0, 4, 7, -1},          /* major */
        {0, 3, 6, -1},          /* diminished */
    };
    int n = 0, k;
    for (k = 0; k < 4; k++)
        if (IV[shape][k] >= 0 && n < SN)
            nt[n++] = (uint8_t)(root + IV[shape][k]);
    return n;
}
/* the progression onto the conductor's pattern: a step a beat (scale 4), a chord at each bar's first step held a bar,
 * in key `key` (0..11) around C4. Returns the scale it wants on the conductor (BS_*) */
int lib_prog_load(tpat_t *tp, int i, int key)
{
    const prog_t *p = &PROGS[((i % NPROGS) + NPROGS) % NPROGS];
    int k, b;
    for (k = 0; k < NSTEP; k++) {
        memset(&tp->st[k], 0, sizeof tp->st[k]);
        tp->st[k].vel = 100;
    }
    for (b = 0; b < p->nbars && b < 8; b++) {
        uint8_t nt[SN];
        int root = 60 + (key % 12) + (p->c[b] & 15), n;
        if (root > 71)
            root -= 12;
        n = chord_notes(root, p->c[b] >> 4, nt);
        step_set_notes(&tp->st[b * 4], nt, n, 100);
        tp->st[b * 4].len = (uint8_t)step_len_code(16);   /* a bar: four beat-steps */
    }
    tp->len = (uint8_t)(p->nbars * 4);
    tp->scale = TS_4;
    tp->lstart = tp->lend = 0;
    return p->scale;
}
