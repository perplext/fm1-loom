/* SPDX-License-Identifier: MPL-2.0 */
/* LOOM-8's library: ready-made beats for the kit and chord progressions for the conductor, by genre, to turn
 * through and load with one press (the Steps list: BEAT on a kit track, PROGRESSION on the conductor; the host:
 * beat / prog). A beat is a string a sound: '.' nothing, 'x' a hit (100), 'X' hard (127), 'o' softer (80),
 * 'g' a ghost (45), '|' a bar line (ignored); 16, 32 or 64 steps at a sixteenth each, the pattern's length set to
 * fit. A progression is a scale and up to eight bars of chords, each a root (semitones above the key) and a
 * shape; it goes on the conductor's pattern a bar a beat-step (its scale 4: a step a beat), one chord a bar, so
 * a track that follows the conductor moves with it. Both only load; the player edits them after as any pattern. */
#include "library.h"
#include <string.h>

/* ---- beats: the sounds in the default kit's order: kick, snare, hat, open, clap, ltom, htom, conga, shaker */
enum { LB_KICK, LB_SNARE, LB_HAT, LB_OPEN, LB_CLAP, LB_LTOM, LB_HTOM, LB_CONGA, LB_SHAKER, LB_N };
typedef struct {
    const char *name, *genre;
    const char *s[LB_N];            /* a string a sound, 0 none */
} beat_t;
static const beat_t BEATS[] = {
    /* dungeon synth: slow, martial, sparse */
    {"crypt march", "dungeon", {"x.......x.......", "........x.......", 0, 0, 0, "..x...x...x...x.", 0, 0, "x.x.x.x.x.x.x.x."}},
    {"torchlight", "dungeon", {"x.......", "....x...", 0, 0, 0, "......x.", 0, 0, 0}},
    {"war drums", "dungeon", {"x..x..x.x..x..x.", "....x.......x...", 0, 0, 0, "x..x..x.x..x..x.", "......x.......x.", 0, 0}},
    {"procession", "dungeon", {"x...x...x...x...", "........x.......", "..x...x...x...x.", 0, 0, 0, 0, 0, "x.x.x.x.x.x.x.x."}},
    {"tambour", "dungeon", {"x.x.....x.x.....", "....x.......x...", 0, 0, 0, "x.x.....x.x.....", "....x.......x...", "x...x...x...x...", 0}},
    /* darkwave / synthwave: machine drums, 80s */
    {"darkwave", "darkwave", {"x...x...x...x...", "....x.......x...", "x.x.x.x.x.x.x.x.", "..............x.", 0, 0, 0, 0, 0}},
    {"coldwave", "darkwave", {"x.......x..x....", "....X.......X...", "xxxxxxxxxxxxxxxx", 0, "....x.......x...", 0, 0, 0, 0}},
    {"gated snare", "darkwave", {"x.....x.x.......", "....X.......X...", "x.x.x.x.x.x.x.x.", 0, 0, "..............xx", "............xx..", 0, 0}},
    {"minimal wave", "darkwave", {"x...x...x...x...", "....x.......x...", "..x...x...x...x.", 0, "....x.......x...", 0, 0, 0, 0}},
    /* blackgaze / doom / metal */
    {"blast", "blackgaze", {"x.x.x.x.x.x.x.x.", ".x.x.x.x.x.x.x.x", "xxxxxxxxxxxxxxxx", 0, 0, 0, 0, 0, 0}},
    {"hyperblast", "blackgaze", {"xxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxx", "x.x.x.x.x.x.x.x.", 0, 0, 0, 0, 0, 0}},
    {"d-beat", "blackgaze", {"x.x.x.x.x.x.x.x.", "..x...x...x...x.", "x.x.x.x.x.x.x.x.", 0, 0, 0, 0, 0, 0}},
    {"doom", "blackgaze", {"x.....x.x.......", "........X.......", "x.x.x.x.x.x.x.x.", "..............x.", 0, 0, 0, 0, 0}},
    {"doom slow", "blackgaze", {"x...............x.......x.......", "................X...............", "x...x...x...x...x...x...x...x...", "............................x...", 0, "..............x.", 0, 0, 0}},
    {"skank", "blackgaze", {"x...x...x...x...", "..x...x...x...x.", "x.x.x.x.x.x.x.x.", 0, 0, 0, 0, 0, 0}},
    {"tremolo roll", "blackgaze", {"x.......x.......", "....x.......x...", "x.x.x.x.x.x.x.x.", 0, 0, "..xx..xx..xx..xx", 0, 0, 0}},
    /* dubstep / drumstep / halftime bass music (140 / 170 halftime) */
    {"dubstep", "dubstep", {"x.........x.....", "........X.......", "x.x.x.x.x.x.x.x.", "......x.........", 0, 0, 0, 0, 0}},
    {"dubstep 2", "dubstep", {"x.......x.x.....", "........X.....g.", "x.xxx.x.x.xxx.x.", 0, "........x.......", 0, 0, 0, 0}},
    {"halftime", "dubstep", {"x......x..x.....", "........X.......", "..x...x...x...x.", 0, 0, 0, 0, 0, "x.x.x.x.x.x.x.x."}},
    {"drumstep", "drumstep", {"x.........x.....", "........X......g", "x.x.x.x.x.x.x.x.", 0, 0, 0, 0, 0, 0}},
    {"drumstep 2", "drumstep", {"x..x......x..x..", "........X.......", "xxxxxxxxxxxxxxxx", "..............x.", 0, 0, 0, 0, 0}},
    {"trap", "trap", {"x.....x...x.....", "........X.......", "x.x.x.xxx.x.x.xx", 0, "........x.......", 0, 0, 0, 0}},
    {"two step", "dnb", {"x.........x.....", "....X.......X...", "x.x.x.x.x.x.x.x.", 0, 0, 0, 0, 0, 0}},
    {"amen", "dnb", {"x.x.......xx....", "....X..g.g..X...", "x.x.x.x.x.x.x.x.", 0, 0, 0, 0, 0, 0}},
    {"jungle", "dnb", {"x.........x.x...", "....X..g..g.X...", "xxxxxxxxxxxxxxxx", 0, 0, 0, 0, 0, 0}},
    /* house / techno / hip hop, for the rest */
    {"four floor", "house", {"x...x...x...x...", "....x.......x...", "..x...x...x...x.", 0, 0, 0, 0, 0, 0}},
    {"techno", "techno", {"x...x...x...x...", 0, "x.x.x.x.x.x.x.x.", "..x...x...x...x.", "....x.......x...", 0, 0, 0, 0}},
    {"dub techno", "techno", {"x...x...x...x...", 0, "..x...x...x...x.", 0, "....x.......x...", 0, 0, "x...x...x...x...", 0}},
    {"boom bap", "hiphop", {"x.....x.x.......", "....X.......X...", "x.x.x.x.x.x.x.x.", 0, 0, 0, 0, 0, 0}},
    {"breakbeat", "breaks", {"x.........x.x...", "....X.g.....X...", "x.x.x.x.x.x.x.x.", "..............x.", 0, 0, 0, 0, 0}},
};
#define NBEATS ((int)(sizeof BEATS / sizeof BEATS[0]))

int lib_nbeats(void) { return NBEATS; }
const char *lib_beat_name(int i) { return BEATS[((i % NBEATS) + NBEATS) % NBEATS].name; }
const char *lib_beat_genre(int i) { return BEATS[((i % NBEATS) + NBEATS) % NBEATS].genre; }

static int beat_vel(char c)
{
    return c == 'x' ? 100 : c == 'X' ? 127 : c == 'o' ? 80 : c == 'g' ? 45 : (c >= '1' && c <= '9') ? (c - '0') * 14 : 0;
}
/* the beat into a kit track's pattern: its steps replaced, its length set, a sixteenth a step. Returns the length */
int lib_beat_load(tpat_t *tp, int i)
{
    const beat_t *b = &BEATS[((i % NBEATS) + NBEATS) % NBEATS];
    int k, len = 0;
    for (k = 0; k < NSTEP; k++) {
        memset(&tp->st[k], 0, sizeof tp->st[k]);
        tp->st[k].vel = 100;
    }
    for (k = 0; k < LB_N; k++) {
        const char *q = b->s[k];
        int s = 0;
        if (!q)
            continue;
        for (; *q; q++) {
            int vel = beat_vel(*q);
            if (*q == '|')
                continue;
            if (s >= NSTEP)
                break;
            if (vel) {
                step_t *st = &tp->st[s];
                kit_add(st, k + 1);
                if (vel > st->vel || kit_mask(st) == 1u << k)   /* the loudest sound sets the step's velocity */
                    st->vel = (uint8_t)vel;
            }
            s++;
        }
        if (s > len)
            len = s;
    }
    tp->len = (uint8_t)(len < 1 ? 16 : len);
    tp->scale = TS_1;
    tp->lstart = tp->lend = 0;
    return tp->len;
}

/* ---- progressions: a scale, a key offset the player keeps (the conductor's key), chords a bar */
enum { CH_TRIAD, CH_POWER, CH_SUS2, CH_SUS4, CH_MIN7, CH_MAJ7, CH_DOM7, CH_ADD9, CH_MIN, CH_MAJ, CH_DIM };
typedef struct { int8_t root; uint8_t shape; } chord_t;
typedef struct {
    const char *name, *genre;
    uint8_t scale;                  /* BS_* */
    uint8_t nbars;
    chord_t c[8];
} prog_t;
#define T(r) {r, CH_TRIAD}
static const prog_t PROGS[] = {
    /* dungeon synth: modal, slow */
    {"i VI III VII", "dungeon", BS_MINOR, 4, {T(0), T(8), T(3), T(10)}},
    {"i VII VI VII", "dungeon", BS_MINOR, 4, {T(0), T(10), T(8), T(10)}},
    {"i iv i v", "dungeon", BS_MINOR, 4, {T(0), T(5), T(0), T(7)}},
    {"i bII i v", "dungeon", BS_PHRYGIAN, 4, {T(0), T(1), T(0), {7, CH_POWER}}},
    {"i VI VII i", "dungeon", BS_MINOR, 4, {T(0), T(8), T(10), T(0)}},
    {"dorian lift", "dungeon", BS_DORIAN, 4, {T(0), T(5), T(10), T(0)}},
    {"harm minor", "dungeon", BS_HARM_MINOR, 4, {T(0), T(8), T(7), T(0)}},
    {"i VI i VII", "dungeon", BS_MINOR, 4, {T(0), T(8), T(0), T(10)}},
    /* darkwave: minor pop shapes */
    {"vi IV I V", "darkwave", BS_MINOR, 4, {T(0), T(8), T(3), T(10)}},
    {"i VII VI V", "darkwave", BS_HARM_MINOR, 4, {T(0), T(10), T(8), T(7)}},
    {"i III VII iv", "darkwave", BS_MINOR, 4, {T(0), T(3), T(10), T(5)}},
    {"i v VI III", "darkwave", BS_MINOR, 4, {T(0), T(7), T(8), T(3)}},
    {"sus two", "darkwave", BS_MINOR, 4, {{0, CH_SUS2}, {8, CH_SUS2}, {10, CH_SUS2}, {5, CH_SUS2}}},
    /* blackgaze / doom: power chords, phrygian, tritones */
    {"phrygian 5ths", "blackgaze", BS_PHRYGIAN, 4, {{0, CH_POWER}, {1, CH_POWER}, {10, CH_POWER}, {8, CH_POWER}}},
    {"i VI III VII 5", "blackgaze", BS_MINOR, 4, {{0, CH_POWER}, {8, CH_POWER}, {3, CH_POWER}, {10, CH_POWER}}},
    {"tritone drop", "blackgaze", BS_MINOR, 4, {{0, CH_POWER}, {6, CH_POWER}, {5, CH_POWER}, {0, CH_POWER}}},
    {"doom descent", "blackgaze", BS_MINOR, 4, {{0, CH_POWER}, {10, CH_POWER}, {8, CH_POWER}, {7, CH_POWER}}},
    {"shoegaze", "blackgaze", BS_MAJOR, 4, {{0, CH_ADD9}, {9, CH_MIN7}, {5, CH_MAJ7}, {7, CH_SUS4}}},
    {"dim rise", "blackgaze", BS_HARM_MINOR, 4, {{0, CH_MIN}, {11, CH_DIM}, {0, CH_MIN}, {8, CH_MAJ}}},
    /* dubstep / drumstep / bass: minor, dark */
    {"i VI VII", "dubstep", BS_MINOR, 4, {T(0), T(8), T(10), T(0)}},
    {"i iv VI v", "dubstep", BS_MINOR, 4, {T(0), T(5), T(8), T(7)}},
    {"minor 7ths", "dubstep", BS_MINOR, 4, {{0, CH_MIN7}, {8, CH_MAJ7}, {3, CH_MAJ7}, {10, CH_DOM7}}},
    {"i bII", "dubstep", BS_PHRYGIAN, 2, {{0, CH_POWER}, {1, CH_POWER}}},
    {"liquid", "dnb", BS_DORIAN, 4, {{0, CH_MIN7}, {5, CH_DOM7}, {10, CH_MAJ7}, {3, CH_MAJ7}}},
    /* house / general */
    {"I V vi IV", "pop", BS_MAJOR, 4, {T(0), T(7), T(9), T(5)}},
    {"ii V I", "jazz", BS_MAJOR, 4, {{2, CH_MIN7}, {7, CH_DOM7}, {0, CH_MAJ7}, {0, CH_MAJ7}}},
    {"deep house", "house", BS_DORIAN, 4, {{0, CH_MIN7}, {0, CH_MIN7}, {5, CH_MIN7}, {10, CH_MAJ7}}},
};
#define NPROGS ((int)(sizeof PROGS / sizeof PROGS[0]))

int lib_nprogs(void) { return NPROGS; }
const char *lib_prog_name(int i) { return PROGS[((i % NPROGS) + NPROGS) % NPROGS].name; }
const char *lib_prog_genre(int i) { return PROGS[((i % NPROGS) + NPROGS) % NPROGS].genre; }
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
        int root = 60 + (key % 12) + p->c[b].root, n;
        if (root > 71)
            root -= 12;
        n = chord_notes(root, p->c[b].shape, nt);
        step_set_notes(&tp->st[b * 4], nt, n, 100);
        tp->st[b * 4].len = (uint8_t)step_len_code(16);   /* a bar: four beat-steps */
    }
    tp->len = (uint8_t)(p->nbars * 4);
    tp->scale = TS_4;
    tp->lstart = tp->lend = 0;
    return p->scale;
}
