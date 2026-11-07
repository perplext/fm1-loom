# SPDX-License-Identifier: MPL-2.0
"""wall's presets: a guitar wall in one voice (three saws, a fizz of noise, a folder, a cabinet) for blackgaze and doom."""
from lib import P

# its sound in a new project: a tremolo-picked wall, driven, dark enough to sit under a lead, wide, the hall on it
P("wall", "strings", "wall", knobs=dict(drive=70, detune=45, noise=25, dark=55), volume=84,
  amp=(8, 70, 120, 55), fenv=(0, 64, 0, 64), filter=("lp12", 110, 5, 64, 64), sends=(15, 60), genres=("lofi",))
# the doom wall: a slower swell, more fold, darker cab, less fizz; power chords an octave down
P("doom wall", "strings", "wall", knobs=dict(drive=105, detune=35, noise=12, dark=80), volume=88,
  amp=(25, 70, 125, 70), fenv=(0, 64, 0, 64), filter=("lp12", 100, 10, 64, 64), sends=(10, 50), genres=("lofi",))
# blown out: everything up, the cassette top end, a tremolo wobble on the volume at 1/32 for tremolo picking
P("blown out", "strings", "wall", knobs=dict(drive=127, detune=60, noise=55, dark=40), volume=78,
  amp=(2, 70, 127, 45), fenv=(0, 64, 0, 64), filter=("lp12", 115, 5, 64, 64), sends=(20, 70),
  lfo=("wobble", 6, 64, 110, 0, 3), genres=("lofi",))
