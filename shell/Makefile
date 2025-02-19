SOURCES = startup.s 
SOURCES += main.cc
SOURCES += ../drivers/console.cc
SOURCES += ../lib/anchor-console/src/console.c
SOURCES += ../drivers/serial.cc
SOURCES += ../drivers/init.cc
SOURCES += ../system/libc/read_write.c
SOURCES += ../system/libc/sbrk.cc
SOURCES += ../system/libc/libc_stub.c

INCLUDES  = -I.
INCLUDES += -I..

INCLUDES += -I../drivers/mdrivlib
INCLUDES += -I../drivers/mdrivlib/target/rk3566
INCLUDES += -I../drivers/mdrivlib/target/aarch64

INCLUDES += -I../lib/anchor-console/include

EXTRA_ARCH_CFLAGS = -DCONSOLE_RETURN_KEY=13
EXTRA_ARCH_CFLAGS += -DCONSOLE_HISTORY

# ignore this static assert failure.
include ../makefile-common.mk

patch:
	@echo "Patching anchor"
	sed -i '14s/.*/\/\//' anchor/console/include/anchor/console/console_private_helpers.h
.PHONY=patch;

