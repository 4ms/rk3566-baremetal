SOURCES = boot.S
SOURCES += ../lib/armv8-bare-metal/vector.S

SOURCES += main.cc

SOURCES += ../lib/armv8-bare-metal/aarch64.c
SOURCES += ../lib/armv8-bare-metal/exception.cc

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

# Shell:
EXTRA_ARCH_CFLAGS = -DCONSOLE_RETURN_KEY=13
EXTRA_ARCH_CFLAGS += -DCONSOLE_HISTORY
SOURCES += ../drivers/console.cc
SOURCES += ../lib/anchor-console/src/console.c
INCLUDES += -I../lib/anchor-console/include

include ../makefile-common.mk

