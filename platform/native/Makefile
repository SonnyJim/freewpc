# This Makefile is used to control a native build, where the code
# runs on the same machine as the build.  It is slightly different
# because of toolchain differences, and also simulation files need
# to be linked in.

D := platform/native
P := platform/wpc
NATIVE_OBJS :=

# Common native mode CPU configuration
CPU := native
CONFIG_PTH := y
CONFIG_CURSES := y
CONFIG_SOFT_REALTIME := y
include cpu/$(CPU)/Makefile

# Subplatform support
# Eventually, this section will contain conditionals for building on the
# different WPC variations.  For now, all of these are mandatory though.
CONFIG_WPC=y
CONFIG_FONT ?= $(CONFIG_DMD)
CONFIG_AC=y
$(eval $(call have,CONFIG_RTC))
CONFIG_ANIMATION ?= $(CONFIG_DMD)
CONFIG_SCORE_RANK := y

# Platform dependent files
KERNEL_HW_OBJS += $(P)/interrupt.o
KERNEL_HW_OBJS += $(P)/init-wpc.o

# Additional defines
CFLAGS += -I$(D) -DCONFIG_PLATFORM_WPC -DPAGE=0

# Additional object files to be linked into the kernel region
NATIVE_OBJS += $(D)/main.o $(D)/switch.o $(D)/task.o $(D)/section.o \
	$(D)/bcd_string.o \
	$(D)/timing.o $(D)/watchdog.o $(D)/zerocross.o \
	$(D)/ball.o $(D)/signal.o $(D)/hwtimer.o $(D)/sound.o \
	$(D)/coil.o $(D)/script.o $(D)/conf.o

# For ASCII DMD
NATIVE_OBJS += $(if $(CONFIG_DMD), $(D)/asciidmd.o, $(D)/segment.o)
NATIVE_OBJS += $(if $(CONFIG_DMD), tools/imglib/imglib.o)
$(D)/asciidmd.o : CFLAGS += -Itools/imglib

# For Ubuntu 8.10 and higher: The default compiler flags will try to
# detect buffer overflows, but we are doing ugly things to read/write
# persistent memory.  We need to disable this 'feature' for this file
# only.
$(D)/section.o : CFLAGS += -U_FORTIFY_SOURCE

$(NATIVE_OBJS) : CFLAGS += -DNATIVE_SYSTEM

# Add machine type flags
CFLAGS += $(if $(CONFIG_DMD), -DMACHINE_DMD=1)

CFLAGS += -DMACHINE_SHORTNAME=\"$(MACHINE)\"

ifeq ($(CONFIG_DMD),y)
IMAGE_MAP += platform/wpc/wpc.ild
endif

# CALLSET_ENTRYs in the simulator directory work too!
CALLSET_FLAGS += -D $(D)

FIRST_BANK=0

# Invoke 'make attach' to start up a GDB session that attaches itself to
# a running instance of the native mode program.
attach:
	gdb -p `ps -ef | grep $(NATIVE_PROG) | head -n 1 | grep -o "[0-9].*" | awk '{print $$1}'` -x gdbmacros
