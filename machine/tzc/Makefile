
#
# Game specific Makefile extensions
#



MACHINE_FILE := tz.md
MACHINE_SCHEDULE := $(M)/tz.sched

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = ftzc

# Set ROM_PAGE_COUNT to the number of 16KB pages
# that should be present in the output image.
# The hardware supports a maximum of 64 pages;
# you can't exceed this.
#
# The 'system' will always reside in the uppermost
# two pages.  If the ROM is less than a full 64
# pages, then the pages are numbered from [N..64]
# anyway.
#
# 128KB.....8 pages
# 256KB.....16 pages
# 512KB.....32 pages
# 1MB...... 64 pages
#
ROM_PAGE_COUNT = 32

#################################################
#
# Simulation options.
#
# These options control the way that the game
# ROM is made available to pinmame for testing.
# They are only used during installation into
# the pinmame roms directory.
#
#################################################

MACHINE_MAJOR = 1
MACHINE_MINOR = 0

PINMAME_MACHINE = tz_92

PINMAME_GAME_ROM = tzone9_2.rom

PINMAME_OTHER_ROMS = tzu*.rom


CONFIG_MUTE_PAUSE := y
EXTRA_CFLAGS += -DCONFIG_MUTE_PAUSE


#
# Object files exported by this game
#
# GAME_OBJS go into the system page, which should be a last resort or for common
# functions, or for those which have to for some reason.  All feature tests can
# go into GAME_TEST_OBJS.  Preferably, game functions go into GAME_PAGED_OBJS.
# This works safely for any file that only uses CALLSET_ENTRY functions, deffs,
# and leffs.  RTTs must go into GAME_OBJS.
#

SCHED_HEADERS += build/left_sling.h build/right_sling.h build/left_jet.h \
	build/right_jet.h build/lower_jet.h build/shooter_div.h 

GAME_OBJS = config.o rocket.o magnet.o slot.o piano.o camera.o deadend.o rightramp.o \
	skill.o leffs.o autofire.o leftramp.o challenge.o lock.o

GAME_TEST_OBJS =

GAME_INCLUDES =

GAME_PAGED_OBJS =

IMAGE_MAP += $(M)/tz.ild

SCHED_HEADERS += build/bridge_open.h
