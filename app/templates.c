/* SPDX-License-Identifier: MPL-2.0 */
/* LOOM-8's genre templates: one press sets a whole project up to play in a style, from the library (library.c) and
 * the factory presets: the tempo and kit, three scenes of beats, a progression in a key, a sound and a role on each
 * track, the two effect tracks, the master. HOME > SEL > TEMPLATE turns through them; SEL asks, then replaces the
 * project (as NEW PROJECT does). The player edits everything after: a template is a starting point. */
#include "library.h"
#include "preset_ids.h"                 /* PI_<ENGINE>_<NAME>: PRESETS' indices (tools/gen_presets.py) */

#define TPL_NSCENE 3
typedef struct {
    uint8_t preset;                       /* a factory preset, PI_* */
    uint8_t follow;                       /* BM_* */
    uint8_t level, send1, send2;          /* 255 = the preset's */
    uint8_t part;                         /* what it plays, PT_* (its pattern 1, in the key; the conductor moves it) */
    uint8_t so, sn;                       /* its knob overrides: sn of TPL_SETS from so */
} tpl_trk_t;
#define TPL_NTRK 4                        /* tracks 2..5 (track 1 is the kit) */
#define SET(p, v) ((uint16_t)((p) << 7 | (v)))   /* a knob override: the parameter in the high bits, its value (0..127) below */
/* the parts: the root held a bar, the root in eighths, a triad held a bar (a harmony follower makes it the chord),
 * a riff of sixteenths on the scale's first five degrees, a held triad through the arp player, the root in a halftime
 * reggae-ish skank (offbeat eighths), a tremolo-picked root (sixteenths, a multiply on each) */
enum { PT_NONE, PT_ROOT, PT_ROOT8, PT_TRIAD, PT_RIFF, PT_ARP, PT_OFFBEAT, PT_TREMOLO };
struct tpl {
    const char *name, *genre;
    uint16_t tempo_x10;
    uint8_t kit;                          /* a factory kit 0..NFKITS-1 */
    uint8_t beat[TPL_NSCENE];             /* a library beat a scene, 1 + its index (library.c BEATS; 0 = the scene's pattern stays empty) */
    uint8_t prog;                         /* a library progression, 1 + its index (PROGS; 0 none) */
    uint8_t key;                          /* 0..11 */
    uint8_t song[TPL_NSCENE];             /* bars a scene in the song (0 = not in the song) */
    tpl_trk_t t[TPL_NTRK];                /* tracks 2..5, in order */
    uint8_t fx1, fx2;                     /* NFXPICK picks */
    uint8_t fx1k[4], fx2k[4];
    uint8_t mst[NMS];                     /* the master: lo mid hi - color camt ctone cmix perc mel comp pump pumpby rel acc level */
};

#define FXP_DELAY FXT_DELAY
#define FXP_CHORUS FXT_CHORUS
#define FXP(mode) (FXT_REVERB + (mode))
#define MST_FLAT {64, 64, 64, 0, MC_OFF, 64, 64, 127, 100, 100, 30, 0, 0, 60, 0, 100}
static const uint16_t TPL_SETS[] = {
    SET(TP_OCT, 1), SET(TP_OCT, 1), SET(TP_CTYPE, CT_DRIVE), SET(TP_CAMT, 80),
    SET(TP_CMIX, 90), SET(TP_OCT, 2), SET(TP_LTYPE, LT_WOBBLE), SET(TP_LSPEED, 6),
    SET(TP_LDEST, 127), SET(TP_LX2, 3), SET(TP_CTYPE, CT_DRIVE), SET(TP_CAMT, 90),
    SET(TP_OCT, 2), SET(TP_OCT, 1), SET(TP_OCT, 3), SET(TP_A, 90),
    SET(TP_R, 100), SET(TP_A, 100), SET(TP_R, 110), SET(TP_CTYPE, CT_TAPE),
    SET(TP_CAMT, 70), SET(TP_CMIX, 100), SET(TP_A, 40), SET(TP_R, 60),
    SET(TP_R, 60), SET(TP_A, 60), SET(TP_R, 100), SET(TP_OCT, 1),
    SET(TP_OCT, 1), SET(TP_CTYPE, CT_OFF), SET(TP_CTYPE, CT_TAPE), SET(TP_CAMT, 40),
    SET(TP_LTYPE, LT_WAVE), SET(TP_LSPEED, 4), SET(TP_LAMT, 127), SET(TP_LDEST, 2),
    SET(TP_LX, 0), SET(TP_RES, 60), SET(TP_OCT, 1), SET(TP_LTYPE, LT_WAVE),
    SET(TP_LSPEED, 5), SET(TP_LAMT, 110), SET(TP_LDEST, 2), SET(TP_LTYPE, LT_WAVE),
    SET(TP_LSPEED, 3), SET(TP_LAMT, 120), SET(TP_LDEST, 2), SET(TP_LX, 0),
    SET(TP_OCT, 1), SET(TP_LSPEED, 5), SET(TP_OCT, 1), SET(TP_OCT, 1),
};
static const tpl_t TEMPLATES[] = {
    {"doomgaze", "blackgaze", 1600, 3, {13, 10, 11}, 17, 4, {4, 2, 2},  /* doom / blast / hyperblast; doom descent */
     {{PI_WALL_DOOM_WALL, BM_TRANSPOSE, 96, 25, 75, PT_TRIAD, 0, 1},
      {PI_KNIT_ROUND_SUB, BM_TRANSPOSE, 108, 0, 0, PT_ROOT, 1, 4},
      {PI_WALL_BLOWN_OUT, BM_HARMONY, 86, 20, 70, PT_TRIAD, 5, 1},
      {PI_KNIT_SOFT_LEAD, BM_HARMONY, 78, 40, 80, PT_TREMOLO, 6, 6}},
     FXP_DELAY, FXP(RV_SHIMMER), {72, 64, 50, 70}, {100, 112, 75, 40},
     {80, 70, 64, 0, MC_TAPE, 105, 60, 100, 104, 100, 60, 0, 0, 60, 0, 108}},
    {"blackgaze", "blackgaze", 1900, 3, {10, 15, 11}, 14, 4, {4, 4, 4},  /* blast / skank / hyperblast; phrygian 5ths */
     {{PI_WALL_WALL, BM_TRANSPOSE, 92, 25, 75, PT_TRIAD, 12, 1},
      {PI_KNIT_ROUND_SUB, BM_TRANSPOSE, 105, 0, 0, PT_ROOT8, 13, 1},
      {PI_WALL_BLOWN_OUT, BM_HARMONY, 80, 40, 80, PT_TRIAD, 14, 1},
      {PI_VOILE_OOH_CHOIR, BM_HARMONY, 70, 20, 90, PT_ROOT, 15, 2}},
     FXP_DELAY, FXP(RV_SHIMMER), {72, 64, 50, 70}, {100, 112, 80, 40},
     {72, 64, 64, 0, MC_TAPE, 70, 70, 100, 100, 100, 45, 0, 0, 60, 0, 104}},
    {"dungeon", "dungeon", 700, 0, {1, 2, 4}, 1, 2, {4, 4, 4},  /* crypt march / torchlight / procession; i VI III VII */
     {{PI_VOILE_OOH_CHOIR, BM_HARMONY, 95, 0, 90, PT_TRIAD, 17, 5},
      {PI_BELLOWS_CHURCH, BM_HARMONY, 80, 20, 60, PT_TRIAD, 22, 2},
      {PI_CANE_NYLON, BM_DIATONIC, 92, 55, 45, PT_ARP, 24, 1},
      {PI_KNIT_ROUND_SUB, BM_TRANSPOSE, 95, 0, 0, PT_ROOT, 25, 3}},
     FXP_DELAY, FXP(RV_SHIMMER), {88, 64, 60, 45}, {110, 118, 45, 70},
     {70, 60, 56, 0, MC_TAPE, 50, 50, 90, 100, 100, 30, 0, 0, 60, 0, 100}},
    {"darkwave", "darkwave", 1100, 2, {6, 8, 7}, 9, 9, {4, 4, 4},  /* darkwave / gated snare / coldwave; vi IV I V */
     {{PI_KNIT_FINGER_BASS, BM_TRANSPOSE, 105, 0, 10, PT_ROOT8, 28, 1},
      {PI_KNIT_DUSTY_PAD, BM_HARMONY, 80, 0, 60, PT_TRIAD, 29, 1},
      {PI_RAYON_MELLOW_EP, BM_HARMONY, 84, 35, 45, PT_ARP, 0, 0},
      {PI_KNIT_SOFT_LEAD, BM_DIATONIC, 82, 45, 55, PT_RIFF, 30, 2}},
     FXP_CHORUS, FXP(RV_HALL), {50, 86, 20, 127}, {127, 64, 64, 0},
     {64, 64, 70, 0, MC_TAPE, 35, 80, 100, 100, 100, 40, 0, 0, 60, 0, 104}},
    {"dubstep", "dubstep", 1400, 2, {17, 18, 19}, 20, 5, {4, 4, 4},  /* dubstep / dubstep 2 / halftime; i VI VII */
     {{PI_KNIT_REESE, BM_TRANSPOSE, 100, 0, 10, PT_ROOT, 32, 6},
      {PI_KNIT_ROUND_SUB, BM_TRANSPOSE, 110, 0, 0, PT_ROOT, 38, 1},
      {PI_JACQUARD_GROWL, BM_TRANSPOSE, 90, 10, 20, PT_OFFBEAT, 39, 4},
      {PI_VOILE_WHISPER_PAD, BM_HARMONY, 70, 0, 80, PT_TRIAD, 0, 0}},
     FXP_DELAY, FXP(RV_LUSH), {72, 64, 50, 60}, {70, 80, 56, 48},
     {76, 64, 64, 0, MC_OFF, 64, 64, 127, 104, 100, 55, 60, 0, 60, 0, 108}},
    {"drumstep", "drumstep", 1740, 2, {20, 21, 23}, 21, 7, {4, 4, 4},  /* drumstep / drumstep 2 / two step; i iv VI v */
     {{PI_KNIT_REESE, BM_TRANSPOSE, 100, 0, 10, PT_ROOT, 43, 5},
      {PI_KNIT_ROUND_SUB, BM_TRANSPOSE, 110, 0, 0, PT_ROOT8, 48, 1},
      {PI_KNIT_HOUSE_STAB, BM_HARMONY, 78, 40, 30, PT_OFFBEAT, 0, 0},
      {PI_JACQUARD_WOBBLE, BM_TRANSPOSE, 86, 10, 25, PT_ROOT, 49, 1}},
     FXP_DELAY, FXP(RV_LUSH), {72, 64, 56, 64}, {64, 70, 56, 48},
     {72, 64, 64, 0, MC_OFF, 64, 64, 127, 104, 100, 55, 55, 0, 50, 0, 108}},
    {"jungle", "dnb", 1700, 2, {24, 25, 23}, 24, 9, {4, 4, 4},  /* amen / jungle / two step; liquid */
     {{PI_KNIT_ROUND_SUB, BM_TRANSPOSE, 108, 0, 0, PT_ROOT, 50, 1},
      {PI_KNIT_REESE, BM_TRANSPOSE, 92, 10, 20, PT_ROOT, 0, 0},
      {PI_RAYON_GLASS_PAD, BM_HARMONY, 76, 20, 60, PT_TRIAD, 0, 0},
      {PI_BELLOWS_ORGAN_STAB, BM_HARMONY, 80, 45, 30, PT_OFFBEAT, 0, 0}},
     FXP_DELAY, FXP(RV_HALL), {76, 64, 60, 50}, {110, 64, 64, 0},
     {68, 64, 66, 0, MC_OFF, 64, 64, 127, 102, 100, 50, 30, 0, 60, 0, 106}},
    {"techno", "techno", 1320, 2, {27, 28, 26}, 23, 0, {4, 4, 4},  /* techno / dub techno / four floor; i bII */
     {{PI_KNIT_FINGER_BASS, BM_TRANSPOSE, 100, 0, 10, PT_ROOT8, 51, 1},
      {PI_KNIT_HOUSE_STAB, BM_HARMONY, 80, 50, 40, PT_OFFBEAT, 0, 0},
      {PI_ACID_ACID_LINE, BM_DIATONIC, 88, 30, 20, PT_RIFF, 0, 0},
      {PI_KNIT_DUSTY_PAD, BM_HARMONY, 70, 0, 70, PT_TRIAD, 0, 0}},
     FXP_DELAY, FXP(RV_SPACE_ECHO), {74, 64, 60, 55}, {127, 70, 64, 10},
     {70, 64, 64, 0, MC_OFF, 64, 64, 127, 102, 100, 45, 50, 0, 60, 0, 106}},
};
#define NTEMPLATES ((int)(sizeof TEMPLATES / sizeof TEMPLATES[0]))
static int tpl_count(void) { return NTEMPLATES; }
static const char *tpl_name(const tpl_t *t) { return t->name; }
static const char *tpl_genre(const tpl_t *t) { return t->genre; }
static const tpl_t *tpl_get(int i) { return &TEMPLATES[((i % NTEMPLATES) + NTEMPLATES) % NTEMPLATES]; }

/* the template onto the project: a new project, then its parts (ui.c's helpers: this file is included after it) */
/* a part onto a track's pattern 1 in the key (C3 + key the root; the conductor's follow moves it to the chords) */
static void tpl_part(tpat_t *pt, int part, int key, uint8_t *par)
{
    int root = 48 + key % 12, k;
    uint8_t one[1], tri[3];
    one[0] = (uint8_t)root;
    tri[0] = (uint8_t)root; tri[1] = (uint8_t)(root + 3); tri[2] = (uint8_t)(root + 7);
    for (k = 0; k < NSTEP; k++) {
        memset(&pt->st[k], 0, sizeof pt->st[k]);
        pt->st[k].vel = 100;
    }
    pt->len = 16;
    pt->scale = TS_1;
    pt->lstart = pt->lend = 0;
    switch (part) {
    case PT_ROOT:
        step_set_notes(&pt->st[0], one, 1, 100);
        pt->st[0].len = (uint8_t)step_len_code(64);
        break;
    case PT_ROOT8:
        for (k = 0; k < 16; k += 2) {
            step_set_notes(&pt->st[k], one, 1, k % 4 ? 90 : 110);
            pt->st[k].len = (uint8_t)step_len_code(7);
        }
        break;
    case PT_OFFBEAT:
        for (k = 2; k < 16; k += 4) {
            step_set_notes(&pt->st[k], one, 1, 100);
            pt->st[k].len = (uint8_t)step_len_code(3);
        }
        break;
    case PT_TRIAD:
        step_set_notes(&pt->st[0], tri, 3, 100);
        pt->st[0].len = (uint8_t)step_len_code(64);
        break;
    case PT_ARP:
        step_set_notes(&pt->st[0], tri, 3, 100);
        pt->st[0].len = (uint8_t)step_len_code(64);
        par[TP_PLAYER] = PL_ARP;
        break;
    case PT_RIFF: {                                  /* 1 . 5 . 4 . 3 . 1 . 5 . 7b . 5 .: a riff on the minor scale */
        static const int8_t D[8] = {0, 7, 5, 3, 0, 7, 10, 7};
        for (k = 0; k < 8; k++) {
            one[0] = (uint8_t)(root + 12 + D[k]);
            step_set_notes(&pt->st[k * 2], one, 1, k % 2 ? 84 : 104);
            pt->st[k * 2].len = (uint8_t)step_len_code(6);
        }
        break;
    }
    case PT_TREMOLO:
        for (k = 0; k < 16; k++) {
            one[0] = (uint8_t)(root + 12);
            step_set_notes(&pt->st[k], one, 1, 96);
            pt->st[k].len = (uint8_t)step_len_code(2);
            pt->st[k].comp = SC_MULTIPLY;
            pt->st[k].cval = 1;
        }
        break;
    default: break;
    }
}
static void template_apply(int i)
{
    const tpl_t *tp = tpl_get(i);
    uint8_t img[KIT_LEN];
    int sc, t, k, scale = -1;
    engine_transport(0);
    engine_panic();
    project_defaults();
    s_cat(proj.name, tp->name);
    proj.tempo_x10 = tp->tempo_x10;
    after_load();
    /* the kit, and a beat a scene on track 1's patterns 1..3 */
    kit_factory(tp->kit % NFKITS, img);
    kit_apply(0, img, 0);
    for (sc = 0; sc < TPL_NSCENE; sc++) {
        tpat_t *pt = &pats.pat[0][sc];
        if (tp->beat[sc])
            lib_beat_load(pt, tp->beat[sc] - 1);
        for (t = 0; t < NSEQT; t++)
            pats.scene[sc].pat[t] = (uint8_t)(t == 0 ? sc : 0);
        pats.scene[sc].used = 1;
    }
    /* the progression on the conductor, in the key */
    proj.trk[AUX_BRAIN].par[TP_E2] = (uint8_t)(tp->key % 12);
    if (tp->prog)
        scale = lib_prog_load(&pats.pat[AUX_BRAIN][0], tp->prog - 1, tp->key % 12);
    if (scale >= 0)
        proj.trk[AUX_BRAIN].par[TP_E3] = (uint8_t)scale;
    engine_set_all(AUX_BRAIN);
    apply_brain();
    for (sc = 0; sc < TPL_NSCENE; sc++) {
        pats.scene[sc].brain_key = (uint8_t)(tp->key % 12);
        pats.scene[sc].brain_scale = (uint8_t)(scale >= 0 ? scale : BS_MINOR);
    }
    /* the tracks: a preset, its role, its level and sends, the overrides; every track sends notes on its channel */
    for (t = 0; t < TPL_NTRK; t++) {
        const tpl_trk_t *tt = &tp->t[t];
        int trk = t + 1;
        load_preset(trk, &PRESETS[tt->preset]);
        proj.trk[trk].par[TP_BRAIN] = tt->follow;
        if (tt->level != 255) proj.trk[trk].par[TP_LEVEL] = tt->level;
        if (tt->send1 != 255) proj.trk[trk].par[TP_SEND1] = tt->send1;
        if (tt->send2 != 255) proj.trk[trk].par[TP_SEND2] = tt->send2;
        for (k = 0; k < tt->sn; k++)
            proj.trk[trk].par[(TPL_SETS[tt->so + k] >> 7) % NTP] = (uint8_t)(TPL_SETS[tt->so + k] & 127);
        tpl_part(&pats.pat[trk][0], tt->part, tp->key % 12, proj.trk[trk].par);
        engine_set_all(trk);
    }
    /* the effects and the master */
    proj.trk[AUX_FX1].par[TP_ENGINE] = tp->fx1 % NFXPICK;
    proj.trk[AUX_FX2].par[TP_ENGINE] = tp->fx2 % NFXPICK;
    for (k = 0; k < 4; k++) {
        proj.trk[AUX_FX1].par[TP_E1 + k] = tp->fx1k[k];
        proj.trk[AUX_FX2].par[TP_E1 + k] = tp->fx2k[k];
    }
    apply_fx(AUX_FX1);
    apply_fx(AUX_FX2);
    for (k = 0; k < NMS; k++) {
        proj.master[k] = (uint8_t)(k == MS_PUMP_BY ? tp->mst[k] : clampi(tp->mst[k], 0, master_max(k)));
        engine_master(k, proj.master[k]);
    }
    /* the song: the scenes in order, each its bars */
    pats.song.len = 0;
    for (sc = 0; sc < TPL_NSCENE; sc++)
        if (tp->song[sc] && pats.song.len < NSONG) {
            pats.song.slot[pats.song.len].scene = (uint8_t)sc;
            pats.song.slot[pats.song.len++].rep = tp->song[sc];
        }
    pats.song.loop = 1;
    seq_set_scene(&seq, 0);
    ui.dirty = 1;
    mark_dirty();
    proj_slot = PROJ_NOSLOT;
    for (int s2 = NPROJ - 1; s2 >= 0; s2--)
        if (!project_exists(s2))
            proj_slot = (uint8_t)s2;
    say("TEMPLATE", tp->name);
}
