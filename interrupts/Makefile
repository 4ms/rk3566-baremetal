SOURCES = boot.S
SOURCES += main.cc
SOURCES += interrupt_handler.cc
SOURCES += vector.S

SOURCES += ../drivers/init.cc
SOURCES += ../drivers/serial.cc
SOURCES += ../drivers/mdrivlib/target/rk3566/drivers/irq_init.cc

SOURCES += ../system/libc/read_write.c
SOURCES += ../system/libc/libc_stub.c
SOURCES += ../system/libc/sbrk.cc

INCLUDES  = -I.
INCLUDES += -I../drivers/mdrivlib
INCLUDES += -I../drivers/mdrivlib/target/rk3566
INCLUDES += -I../
INCLUDES += -I../drivers
INCLUDES += -I../lib
INCLUDES += -I../system
INCLUDES += -I../lib/cpputil
INCLUDES += -I../drivers/mdrivlib

OPTFLAG = -O0


# Shell:
EXTRA_ARCH_CFLAGS = -DCONSOLE_RETURN_KEY=13
EXTRA_ARCH_CFLAGS += -DCONSOLE_HISTORY
EXTRA_ARCH_CFLAGS += -DRK3566

SOURCES += ../drivers/console.cc
SOURCES += ../lib/anchor-console/src/console.c
INCLUDES += -I../lib/anchor-console/include

include ../makefile-common.mk

