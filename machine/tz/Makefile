
#
# Game specific Makefile extensions
#

MACHINE_FILE = tz.md

# Define the name of the GAME_ROM.  This is
# optional; you can delete it and a suitable
# default will be taken.
GAME_ROM_PREFIX = ftz

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

PINMAME_MACHINE = tz_92

PINMAME_GAME_ROM = tzone9_2.rom

PINMAME_OTHER_ROMS = tzu*.rom

#
# Object files exported by this game
#

GAME_OBJS = config.o \
	clock.o deffs.o leffs.o \
	amode.o lock.o magnet.o \
	slot.o sling.o jets.o rightramp.o rocket.o star.o \
	greed.o piano.o \
	loop.o bonus.o door.o autofire.o gumball.o \
	clocktarget.o hitch.o ballsave.o powerball.o \
	multiball.o mpf.o

GAME_TEST_OBJS = test.o clocktest.o gumball_test.o magtest.o

GAME_INCLUDES =

# These should eventually move to the machine page!
GAME_PAGED_OBJS = lanes.o leftramp.o skill.o deadend.o camera.o

GAME_FSMS = shot.fsm