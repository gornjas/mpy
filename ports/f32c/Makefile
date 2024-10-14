include ../../py/mkenv.mk

CROSS = 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 0

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

ifeq ($(CROSS), 1)
ifeq ($(ARCH), riscv)
CROSS_COMPILE ?= riscv32-elf-
LDFLAGS += --library-path=${F32C_BASE_DIR}/src/lib/riscv
else
CROSS_COMPILE ?= mips-elf-
LDFLAGS += --library-path=${F32C_BASE_DIR}/src/lib/mipsel
endif
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
F32C_BASE_DIR=/home/m/github/f32c
INC += -I${F32C_BASE_DIR}/src/include
ifeq ($(ARCH), mips)
CFLAGS_F32C = -march=f32c -EL
endif
ifeq ($(ARCH), riscv)
CFLAGS_F32C = -mabi=ilp32 -march=rv32im
endif
CFLAGS_F32C += -nostdinc -include sys/param.h -DF32C
CFLAGS += $(INC) -Wall -Werror -std=c99 -nostdlib $(CFLAGS_F32C) $(COPT)
LIBS += -lm -lc
LDFLAGS += -N -EL
LDFLAGS += --section-start=.init=0x80000000
LDFLAGS += --gc-sections -lcrt0
OBJFLAGS = -R .rel.dyn -R .MIPS.abiflags
DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
ifeq ($(shell uname -s), FreeBSD)
        ISA_CHECK = ${F32C_BASE_DIR}/src/tools/isa_check.tcl
else
        ISA_CHECK = tclsh ${F32C_BASE_DIR}/src/tools/isa_check.tcl
endif
else
LIBS += -lm
LD = $(CC)
CFLAGS += $(INC) -Wall -Werror -Wdouble-promotion -std=c99 $(COPT)
#CFLAGS += -Wfloat-conversion
LDFLAGS += -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif

CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -O0
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))

SRC_C += \
	main.c \
	uart_core.c \
	f32c_mphal.c

SHARED_SRC_C += $(addprefix shared/,\
	libc/printf.c \
	readline/readline.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	$(SHARED_SRC_C_EXTRA) \
	)

EXTMOD_SRC_C += $(addprefix extmod/,\
	modtime.c \
	)

ifeq ($(CROSS), 1)
SRC_C += shared/libc/string0.c
endif

OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))

SRC_QSTR += $(SRC_C) $(SRC_CXX) $(SHARED_SRC_C) $(EXTMOD_SRC_C)

include $(TOP)/py/mkrules.mk


all: $(BUILD)/py.bin

$(BUILD)/py.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/py.bin: $(BUILD)/py.elf
	${ISA_CHECK} $(ARCH) $(BUILD)/py.elf
	$(Q)$(OBJCOPY) -O srec $(OBJFLAGS) $^ $(BUILD)/py.srec
	$(Q)$(OBJCOPY) -O binary $(OBJFLAGS) $^ $(BUILD)/py.bin

# Run emulation build on a POSIX system with suitable terminal settings
run:
	@saved_=`stty -g`; \
	  stty raw opost -echo; \
	  $(BUILD)/firmware.elf; \
	  echo "Exit status: $$?"; \
	  stty $$saved_

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

