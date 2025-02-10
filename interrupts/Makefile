SOURCES = boot.S
SOURCES += ../lib/armv8-bare-metal/vector.S

SOURCES += main.cc
SOURCES += ../system/mmu.cc

SOURCES += ../lib/armv8-bare-metal/aarch64.c
SOURCES += ../lib/armv8-bare-metal/exception.c
SOURCES += ../lib/armv8-bare-metal/gic_v3.c

SOURCES += ../drivers/init.cc
SOURCES += ../drivers/serial.cc
SOURCES += ../system/libc/read_write.c
SOURCES += ../system/libc/libc_stub.c
SOURCES += ../system/libc/sbrk.cc

INCLUDES  = -I.
INCLUDES += -I../drivers
INCLUDES += -I../lib
INCLUDES += -I../system

OPTFLAG = -O0

include ../makefile-common.mk

