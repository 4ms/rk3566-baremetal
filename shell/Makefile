SOURCES = startup.s 
SOURCES += main.cc
SOURCES += ../drivers/serial.cc
SOURCES += anchor/console/src/console.c
SOURCES += libc/read_write.c
SOURCES += libc/sbrk.cc
SOURCES += ../drivers/init.cc
SOURCES += libc/libc_stub.c

INCLUDES  = -I.
INCLUDES += -I../drivers
INCLUDES += -Ianchor/console/include

EXTRACXXFLAGS = -D_Static_assert=static_assert

EXTRA_ARCH_CFLAGS = -DCONSOLE_RETURN_KEY=13
EXTRA_ARCH_CFLAGS += -DCONSOLE_HISTORY

# ignore this static assert failure.
include ../makefile-common.mk

patch:
	@echo "Patching anchor"
	sed -i '14s/.*/\/\//' anchor/console/include/anchor/console/console_private_helpers.h
.PHONY=patch;

