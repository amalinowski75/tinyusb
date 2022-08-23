include ../../../tools/top.mk
include ../../make.mk

LIBOPENCM3_SRC = lib/libopencm3

CFLAGS += -DSTM32F7 -DSTM32F7CCM -DSTM32F730VGT6 -D_ROM=64K -D_RAM=192K -D_CCM=64K -D_CCM_OFF=0x20000000 -D_ROM_OFF=0x08000000 -D_RAM_OFF=0x20010000

LIBNAME = opencm3_stm32f7
LIBS += -l$(LIBNAME)
LDFLAGS += -L$(TOP)/$(LIBOPENCM3_SRC)/lib


INC += \
  src \
  $(TOP)/hw \
  $(TOP)/$(LIBOPENCM3_SRC)/include

# Example source
EXAMPLE_SOURCE += $(wildcard src/*.c)
SRC_C += $(addprefix $(CURRENT_PATH)/, $(EXAMPLE_SOURCE))

include ../../rules.mk
