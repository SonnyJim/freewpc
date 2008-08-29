
# Root directory for the CPU files.  TODO: move
# the code to cpu/m6809 and change $(C) accordingly.
CPU := m6809
C := platform/wpc

CFLAGS =

ifneq ($(GCC_VERSION),3.4.6)
SOFTREG_OPTIONS := -msoft-reg-count=4 -DSOFT_REG_COUNT=4
endif

# Save assembler files for debugging the 6809 compiler.
ifeq ($(SAVE_ASM),y)
CFLAGS += -save-temps
endif

# Turn on compiler debug.  This will cause a bunch of compiler
# debug files to get written out.
ifeq ($(DEBUG_COMPILER),y)
CFLAGS += -da -dA
endif

# Optimization flags
CFLAGS += -O2 -fomit-frame-pointer -fstrength-reduce -frerun-loop-opt -Wunknown-pragmas -foptimize-sibling-calls -fstrict-aliasing -fregmove -fgcse-after-reload -fgcse-sm -fgcse-las -ffreestanding
CFLAGS += -DCONFIG_BIG_ENDIAN -DHAVE_FASTRAM_ATTRIBUTE
# TODO : should -funroll-loops be given?

# Which version of the 6809 compiler to use.
GCC_VERSION ?= 4.2.1

KERNEL_OBJS     += $(C)/vector.o
KERNEL_OBJS     += $(C)/div10.o
KERNEL_ASM_OBJS += $(C)/farcall.o
KERNEL_OBJS     += $(if $(CONFIG_TASK), $(C)/task.o)
KERNEL_ASM_OBJS += $(if $(CONFIG_TASK), $(C)/task_6809.o)
KERNEL_OBJS     += $(if $(CONFIG_MALLOC), $(C)/malloc.o)