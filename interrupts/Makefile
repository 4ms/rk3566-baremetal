SOURCES =

SOURCES += startup.s
SOURCES += main.cc
SOURCES += vector_table.s

SOURCES += ../drivers/init.cc
SOURCES += ../drivers/serial.cc

SOURCES += ../system/libc/read_write.c
SOURCES += ../system/libc/libc_stub.c
SOURCES += ../system/libc/sbrk.cc

INCLUDES  = -I.
INCLUDES += -I../
INCLUDES += -I../lib
INCLUDES += -I../system
INCLUDES += -I../lib/cpputil

# Shell:
EXTRA_ARCH_CFLAGS = -DCONSOLE_RETURN_KEY=13
EXTRA_ARCH_CFLAGS += -DCONSOLE_HISTORY

SOURCES += ../drivers/console.cc
SOURCES += ../lib/anchor-console/src/console.c
INCLUDES += -I../lib/anchor-console/include

# mdrivlib:
SOURCES += ../drivers/mdrivlib/target/rk3566/drivers/irq_init.cc
SOURCES += ../drivers/mdrivlib/target/aarch64/drivers/interrupt_handler.cc

INCLUDES += -I../drivers/mdrivlib
INCLUDES += -I../drivers/mdrivlib/target/rk3566
INCLUDES += -I../drivers/mdrivlib/target/aarch64

EXTRA_ARCH_CFLAGS += -DRK3566

#####
OPTFLAG = -O0

include ../makefile-common.mk

