#--------------------------------------------------------------------------
# Machine description for The Machine: Bride of Pinbot
# (C) Copyright 2007, 2009, 2010, 2011 by Brian Dominy <brian@oddchange.com>
#
# See tools/genmachine for more information about the format of this file.
#--------------------------------------------------------------------------

# This file describes many characteristics of a pinball machine,
# mostly physical stuff, although some rules can be coded here also.

##########################################################################
# General section (before a [section] header is given.
# Miscellaneous parameters are specified here.
##########################################################################
Title: Bride of Pinbot
include platform/wpc/wpc89.md

#Pinmame-Zip:
#Pinmame-ROM:
#Lamp-Matrix-Width:
#Lamp-Matrix-Height:

##########################################################################
# Use 'define' to emit a plain #define for anything not covered by
# some other means.
##########################################################################
#define MACHINE_SYS11_SOUND
#define MACHINE_SCORE_DIGITS
#define MACHINE_MUSIC_GAME
#define MACHINE_MUSIC_PLUNGER
#define MACHINE_REPLAY_SCORE_CHOICES       10
#define MACHINE_REPLAY_START_CHOICE        5
#define MACHINE_BALL_SAVE_TIME             5
#define MACHINE_ONE_BALL_SAVE
define MACHINE_MAX_BALLS                  3

#define MACHINE_CUSTOM_AMODE
define MACHINE_GRAND_CHAMPION_INITIALS { 'L', 'E', 'D' }
define MACHINE_GRAND_CHAMPION_SCORE { 0x00, 0x15, 0x00, 0x00, 0x00 }
define MACHINE_HIGH_SCORE_INITIALS { 'B', 'C', 'D' }, { 'Q', 'Q', 'Q' }, { 'D', 'E', 'H' }, { 'J', 'N', 'D' }
define MACHINE_HIGH_SCORES { 0x00, 0x10, 0x00, 0x00, 0x00 }, { 0x00, 0x09, 0x00, 0x00, 0x00 }, { 0x00, 0x08, 0x00, 0x00, 0x00 }, { 0x00, 0x07, 0x00, 0x00, 0x00 }

##########################################################################
# Lamp Description
# The key is given in column/row format.  The first parameter must be
# the lamp name.  Optionally you can specify a color, and x() and y()
# for the location on the playfield.  Only a subset of lamp colors are
# recognized; see tools/genmachine for details.  Lamp location is
# given in terms of the Lamp-Matrix-Width and Lamp-Matrix-Height.
##########################################################################
[lamps]
11: Left Outlane, y(35), x(5)
12: Left Inlane, y(37), x(10)
13: Right Inlane, y(37), x(65)
14: Right Outlane, y(35), x(72)
15: Left Standup, y(54), x(13)
16: Right Top Standup, y(58), x(60)
17: Right Bottom Standup, y(52), x(62)
18: Shoot Again, shoot-again, y(10), x(32)
21: SW Jackpot Lit, y(50), x(40)
22: BW Lite Billion, y(57), x(40)
23: SW Extra Ball, y(40), x(44)
24: SW Lite Extra Ball, y(37), x(40)
25: SW 50K, y(40), x(33)
26: SW 100K, y45), x(33)
27: Values x2, y(44), x(40)
28: Spin Small Wheel, y(68), x(10)
31: SW 250K, y(45), x(44) 
32: BW 10M, y(49), x(47)
33: BW 50M, y(38), x(50)
34: BW Special, y(32), x(40)
35: BW 5M, y(37), x(27)
36: BW 1M, y(49), x(27)
37: Space Shuttle, y(72), x(20)
38: Launch Pad, y(63), x(21)
41: Skill Shot 50K, y(65), x(75)
42: Skill Shot 75K, y(76), x(75)
43: Skill Shot 100K, y(87), x(75)
44: Skill Shot 200K, y(98), x(75)
45: Skill Shot 25K, y(109), x(75)
46: Left Eye, y(129), x(29)
47: Right Eye, y(129), x(39)
48: Mouth, y(122), x(34)
51: Left Loop 500K, y(80), x(27)
52: Left Loop 100K, y(75), x(28)
53: Left Loop 50K, y(71), x(30)
54: Left Loop 25K, y(67), x(32)
55: Right Loop 500K, y(78), x(62)
56: Right Loop 100K, y(73), x(60)
57: Right Loop 50K, y(69), x(57)
58: Right Loop 25K, y(64), x(56)
61: Right Ramp 1M, y74), x(40)
62: Right Ramp 500K, y(80), x(40)
63: Right Ramp 100K, y(84), x(40)
64: Wire lock ,y(100), x(30)
65: Jets 500K, y(80), x(53)
66: Jets 100K, y(74), x(51)
67: Jets 50K, y(70), x(50)
68: Jets 25K, y(66), x(48)
71: Jackpot 8M, 
72: Jackpot 7M
73: Jackpot 6M
74: Jackpot 5M
75: Jackpot 4M
76: Jackpot 3M
77: Jackpot 2M
78: Jackpot 1M
81: Backglass Hip
82: Backglass Middle Leg
83: Backglass Knee
84: Backglass Foot
85: Backglass Shoulder
86: MPF 100K, y(160), x(66)
87: MPF 200K, y(160), x(73)
88: MPF 300K, y(160), x(78)


##########################################################################
# Switch Description
# The key is in column/row format.  The first parameter must be the switch
# name.  Options can be given in any order:
#    ingame - only service the switch during a game
#    intest - also service the switch in test mode
#    novalid - tripping this switch does NOT mark ball in play
#    standup - this is a standup
#    button - this is a button
#    edge - this switch should be serviced on either transition
#    opto - this switch is optical and activates on closed->open
#
# These parameters mark various well-known switches.  Only one of these
# can exist per type:
#    outhole, slam-tilt, tilt, shooter, start-button, buyin-button
#
# Use sound() to invoke a sound call automatically when the switch activates.
# Use lamp() to flicker a lamp automatically on activation.  TODO:
#   this requires the c_name, and not the friendly md name.
# Use c_decl() to override the default name of the switch event.
#
##########################################################################
[switches]
11: Right Flipper, button, intest, c_decl(sw_right_button)
12: Left Flipper, button, intest, c_decl(sw_left_button)
13: Start Button, start-button, intest
14: Plumb bob tilt, c_decl(sw_tilt), cabinet, tilt, ingame, novalid
15: Left Outlane, ingame
16: Left Inlane, ingame
17: Right Inlane, ingame
18: Right Outlane, ingame
21: Slam Tilt, slam-tilt, ingame, cabinet
23: Ticket Opto, cabinet, opto, novalid
25: Trough Right, noscore, edge, trough
26: Trough Center, noscore, edge, trough
27: Trough Left, noscore, edge, trough
28: Left Standup, standup
31: Skill Shot 50K, edge
32: Skill Shot 75K, ingame
33: Skill Shot 100K, ingame
34: Skill Shot 200K, ingame
35: Skill Shot 25K, ingame
36: Right Top Standup, standup ingame
37: Right Bottom Standup, standup, ingame
38: Outhole, outhole, noscore, novalid
41: Right Ramp Made, ingame
43: Left Loop, ingame,
44: Right Loop Top, ingame
45: Right Loop Bottom, ingame
46: Under Playfield Kickback, edge
47: Enter Head
51: Spinner, ingame
52: Shooter, shooter, edge, novalid, debounce(TIME_200MS)
53: UR Jet, ingame, c_decl(jet_hit)
54: UL Jet, ingame, c_decl(jet_hit)
55: Lower Jet, ingame, c_decl(jet_hit)
56: Jet Sling, ingame, c_decl(jet_hit)
57: Left Sling, ingame
58: Right Sling, ingame
63: Left Eye, intest
64: Right Eye, intest
65: Mouth, intest
67: Face Position, intest, novalid
71: Wire Lock Top, intest
72: Wire Lock Bottom, intest
73: MPF Enter, ingame
74: MPF Exit Left, ingame
75: MPF Exit Right, ingame
76: Left Ramp Enter, ingame
77: Right Ramp Enter, ingame

##########################################################################
# Drives
# This describes the names of the solenoid/motor drives.
# Hn = high power solenoids
# Ln = low power solenoids
# Gn = general purpose solenoids
# etc.
#
# The following options are supported:
#    flash - this is a flasher (default is solenoid, flash implies nosearch)
#    motor - this is a motor (default is solenoid)
#    nosearch - do not activate this during ball search
#
# The following options denote well-known drives:
#    knocker, ballserve
#
##########################################################################
[drives]
H1: Outhole
H2: Trough Release, ballserve
H3: UPF Kicker
H4: MPF Gate
H5: SShot Kicker
H6: Wire Ball Lock, duty(SOL_DUTY_25), time(TIME_100MS)
H7: Knocker, knocker
H8: Mouth, nosearch

L1: UL Jet
L2: Left Sling
L3: UR Jet
L4: Right Sling
L5: Lower Jet
L6: Jet Sling
L7: Left Eye, nosearch
L8: Right Eye, nosearch

G1: Billion, flash
G2: Left Ramp, flash
G3: Jackpot, flash
G4: Skill Shot, flash
G5: Left Helmet, flash
G6: Right Helmet, flash
G7: Jets Enter, flash
G8: Left Loop, flash

A1: Helmet Data, nosearch
A2: Helmet Clock, nosearch
A3: Head Motor Relay, nosearch
A4: Head Motor, motor, nosearch

##########################################################################
# General Illumination
##########################################################################
[gi]
0: Backglass and body
1: Helmet
2: Rear playfield
3: Backglass only
4: Front Playfield
7: Flippers

##########################################################################
# Tests
# These are additional test items that should appear in the TESTS menu.
##########################################################################
[tests]
Head:

##########################################################################
# Lampsets
# These denote logical groupings of lamps.
# The key is a textual name for the set.  The options specify which lamps
# are a part of that set.  You can specify 1 or more lamp values,
# separated by commas, where each value can be: 1) a single lamp name,
# 2) another lampset name, already defined, or 3) a lamp range in the
# form lamp1..lampN.
#
# The special construct PF:function indicates that a Perl subroutine
# should be used to select and sort the lamps.  Some functions are
# builtin to genmachine; others can be defined by you and included
# via the 'perlinclude' directive.
#
# Note that a lampset may contain only one lamp.  The lampset is the
# unit of 'allocation' for a lamp effect.
##########################################################################
[lamplists]
Playfield: PF:all
Lanes: Left Outlane..Right Outlane
Left loops: Left Loop 500K..Left Loop 25K
Right loops: Right Loop 500K..Right Loop 25K
Right Ramps: Right Ramp 100K..Right Ramp 1M
Skill Shot: Skill Shot 50K..Skill Shot 25K
Small Wheel: SW Jackpot Lit, SW 250K, SW Extra Ball,  SW Lite Extra Ball, SW 50K, SW 100K
Big Wheel: BW Lite Billion, BW 10M, BW 50M, BW Special, BW 5M, BW 1M
Jets enter: Jets 500K..Jets 25K
MPF: MPF 100K..MPF 300K
Lower_pf: Left Outlane..Right Bottom Standup
Lanes: Left Outlane..Right Outlane
Left Ramps:Space Shuttle, Launch Pad, Spin Small Wheel

Bottom to top: PF:lamp_sort_bottom_to_top
Top to bottom: PF:lamp_sort_top_to_bottom
LTR: PF:lamp_sort_left_to_right
RTL: PF:lamp_sort_right_to_left
Circle Out: PF:lamp_sort_circle_out
##########################################################################
# Lamp effects
##########################################################################
[leffs]
Amode: page(MACHINE_PAGE)
#Circle Out: PRI_LEFF3, LAMPS(CIRCLE_OUT), page(MACHINE_PAGE)
#Shooter: PRI_LEFF2, page(MACHINE_PAGE)
Skill Strobe: page(MACHINE_PAGE), PRI_LEFF3, LAMPS(SKILL_SHOT)
Strobe Up: page(MACHINE_PAGE), PRI_LEFF2, LAMPS(BOTTOM_TO_TOP)
Jets Completed: page(MACHINE_PAGE), PRI_LEFF3, LAMPS(JETS_ENTER)

[containers]
Trough: Trough Release, trough, Trough Left, Trough Center, Trough Right, init_max_count(3)

Left Eye: Left Eye, init_max_count(0), Left Eye
Right Eye: Right Eye, init_max_count(0), Right Eye
Mouth: Mouth, init_max_count(0), Mouth
Wire Ball Lock: Wire Ball Lock, init_max_count(0), Wire Lock Top, Wire Lock Bottom

UPFKicker: UPF Kicker, init_max_count(0), Under Playfield Kickback

#SShotKicker: SShot Kicker, init_max_count(0), Skill Shot 50K, Skill Shot 75K, Skill Shot 100K, Skill Shot 200K, Skill Shot 25K

#------------------------------------------------------------------------
# The remaining sections describe software aspects, and not the physical
# machine.
#------------------------------------------------------------------------

##########################################################################
# Items for the Feature Adjustments menu.  Parameters indicate the
# type of adjustment and the default value.
##########################################################################
[adjustments]

##########################################################################
# Items for the Feature Audits menu.
##########################################################################
[audits]

##########################################################################
# Sound calls for well-known events
##########################################################################
[system_sounds]
Add Coin: SND_COIN
Tilt Warning: SND_TILT_WARNING
Tilt: SND_TILT

##########################################################################
# Music calls for well-known events
##########################################################################
[system_music]
Start Ball: MUS_MAIN1_PLUNGER
Ball In Play: MUS_MAIN1_1
End Game: MUS_GAME_OVER
Volume Change: MUS_SECRET_FANFARE

##########################################################################
# A list of all scores needed by the game rules.
##########################################################################
[scores]
110:
1K:
2570:
5K:
5130:
10K:
25K:
50K:
75K:
100K:
125K:
150K:
175K:
200K:
225K:
250K:
275K:
300K:
500K:
750K:
1M:


##########################################################################
# Bit flags.
##########################################################################
[flags]
Skill shot enabled:

SW Jackpot:
EB Lit:
50K Skill Shot:
100K Skill Shot:
250K Jets:
SW Completed:

Billion Lit:
EB Collected:
10M Total:
50M Total:
Special Lit:
1M Jets:
BW Completed:


50K SS Made:
75K SS Made:
100K SS Made:
200K SS Made:
25K SS Made:
Skill missed:

[globalflags]
Head Broken:
Super Jackpot Lit:
MB Jackpot Lit:



##########################################################################
# Display effects
# The higher the value, the greater the priority it has
##########################################################################
[deffs]
Skill Shot: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_QUEUED+D_PAUSE
Skill Missed: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_QUEUED+D_PAUSE
Jet Hit: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_RESTARTABLE
#Jets Level Up: page(MACHINE_PAGE), PRI_GAME_MODE3, D_QUEUED
Standup Multiplier: page(MACHINE_PAGE), PRI_GAME_MODE4, D_QUEUED
Loop: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_RESTARTABLE+D_SCORE
Loop combo: page(MACHINE_PAGE), PRI_GAME_QUICK4, D_RESTARTABLE+D_SCORE
Right Ramp: page(MACHINE_PAGE), PRI_GAME_QUICK5, D_RESTARTABLE
Right Ramp combo: page(MACHINE_PAGE), PRI_GAME_QUICK5, D_RESTARTABLE
Shuttle Launch: page(MACHINE_PAGE), PRI_GAME_QUICK5, D_RESTARTABLE
Abort Launch: page(MACHINE_PAGE), PRI_GAME_QUICK6, D_RESTARTABLE
Rollover Completed: page(MACHINE_PAGE), PRI_GAME_QUICK2, D_RESTARTABLE
Wheel: page(MACHINE_PAGE), PRI_GAME_MODE3, D_QUEUED+D_PAUSE
Bonus: page(MACHINE_PAGE), PRI_BONUS
Calibrating Head: page(MACHINE_PAGE), PRI_BALL_SEARCH

Jackpot: page(MACHINE_PAGE), PRI_JACKPOT, D_QUEUED+D_PAUSE
Super Jackpot: page(MACHINE_PAGE), PRI_JACKPOT, D_QUEUED+D_PAUSE
Super Jackpot Value: page(MACHINE_PAGE), PRI_JACKPOT, D_QUEUED+D_PAUSE

I Can Speak: page(MACHINE_PAGE), PRI_GAME_MODE7, D_QUEUED+D_PAUSE
Ball locked: page(MACHINE_PAGE), PRI_GAME_MODE8, D_QUEUED+D_PAUSE


[templates]

Left Sling: driver(spsol), sw=SW_LEFT_SLING, sol=SOL_LEFT_SLING, ontime=4, offtime=20

Right Sling: driver(spsol), sw=SW_RIGHT_SLING, sol=SOL_RIGHT_SLING, ontime=4, offtime=20

Jet Sling: driver(spsol), sw=SW_JET_SLING, sol=SOL_JET_SLING, ontime=4, offtime=20

UL Jet: driver(spsol), sw=SW_UL_JET, sol=SOL_UL_JET, ontime=4, offtime=20

UR Jet: driver(spsol), sw=SW_UR_JET, sol=SOL_UR_JET, ontime=4, offtime=20

Lower Jet: driver(spsol), sw=SW_LOWER_JET, sol=SOL_LOWER_JET, ontime=4, offtime=20

#TODO Doesnt seem to trigger sw_spinner_low
#Spinner: driver(spinner), sw_event=sw_spinner, sw_number=SW_SPINNER

Gate: driver(duty), sol=SOL_MPF_GATE, ontime=TIME_300MS, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=60

Head Motor: driver(duty), sol=SOL_HEAD_MOTOR, ontime=0, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=255
Head Motor Relay: driver(duty), sol=SOL_HEAD_MOTOR_RELAY, ontime=0, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=255

Outhole: driver(outhole), sol=SOL_OUTHOLE, swno=SW_OUTHOLE, swevent=sw_outhole

Wire Ball Lock: driver(duty), sol=SOL_WIRE_BALL_LOCK, ontime=TIME_1S, duty_ontime=TIME_33MS, duty_offtime=TIME_16MS, timeout=1

[system_sounds]
Add Coin: SND_COIN
Add Credit: SND_THANK_YOU
Start Game: SND_STANDBY
Tilt Warning: SND_TILT_WARNING
Tilt: SND_TILT
