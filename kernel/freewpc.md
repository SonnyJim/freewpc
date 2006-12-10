#--------------------------------------------------------------------------
# System description for FreeWPC
# (C) Copyright 2006 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#
# All machine descriptions should "include kernel/freewpc.md" in order to
# get the common definitions that apply to all WPC-era games.
#--------------------------------------------------------------------------

[lamps]
11:
88:

[switches]
D1: Left Coin, cabinet
D2: Center Coin, cabinet
D3: Right Coin, cabinet
D4: Fourth Coin, cabinet
D5: Escape, service, intest, cabinet
D6: Down, service, intest, cabinet
D7: Up, service, intest, cabinet
D8: Enter, service, intest, cabinet
22: Coin Door Closed, cabinet
24: Always Closed, virtual, opto, c_decl(sw_unused)
F1: L. R. Flipper EOS, opto, cabinet
F2: L. R. Flipper Button, button, opto, intest
F3: L. L. Flipper EOS, opto, cabinet
F4: L. L. Flipper Button, button, opto, intest
F8: SW. F8, c_decl(sw_unused)

[drives]
H1: High Power Sol. 1
H2: High Power Sol. 2
H3: High Power Sol. 3
H4: High Power Sol. 4
H5: High Power Sol. 5
H6: High Power Sol. 6
H7: High Power Sol. 7
H8: High Power Sol. 8

L1: Low Power Sol. 1
L2: Low Power Sol. 2
L3: Low Power Sol. 3
L4: Low Power Sol. 4
L5: Low Power Sol. 5
L6: Low Power Sol. 6
L7: Low Power Sol. 7
L8: Low Power Sol. 8

G1: General Sol. 1
G2: General Sol. 2
G3: General Sol. 3
G4: General Sol. 4
G5: General Sol. 5
G6: General Sol. 6
G7: General Sol. 7
G8: General Sol. 8

A1: Aux. Sol. 1
A2: Aux. Sol. 2
A3: Aux. Sol. 3
A4: Aux. Sol. 4
A5: Aux. Sol. 5
A6: Aux. Sol. 6
A7: Aux. Sol. 7
A8: Aux. Sol. 8

[lampsets]
ALL: 11..88

[deffs]
NULL: 0
Inspector: runner, PRI_DEBUGGER
Scores: runner, PRI_SCORES
Coin Insert: PRI_COINS
Credits: PRI_CREDITS
Tilt Warning: PRI_TILT_WARNING
Tilt: runner, PRI_TILT
Game Over: PRI_GAME_OVER
Volume Change: runner, PRI_VOLUME_CHANGE
Slam Tilt: runner, PRI_SLAMTILT
Status Report: runner, PRI_STATUS
Nonfatal Error: PRI_DEBUGGER
HSEntry: runner, PRI_HSENTRY
Match: runner, PRI_MATCH
Locating Balls: PRI_BALL_SEARCH
Buyin Offer: PRI_MATCH

[leffs]
NULL: 0
Tilt Warning: runner, PRI_TILT_WARNING, LAMPS(ALL), c_decl(no_lights_leff)
Tilt: runner, PRI_TILT, LAMPS(ALL), GI(ALL), c_decl(no_lights_leff)

