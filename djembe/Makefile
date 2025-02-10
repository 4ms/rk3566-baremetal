SOURCES = startup.s 
SOURCES += main.cc
SOURCES += mmu.cc

SOURCES += ../lib/armv8-bare-metal/aarch64.c
SOURCES += ../drivers/init.cc
SOURCES += ../drivers/serial.cc
SOURCES += ../drivers/print.cc
SOURCES += ../libc/read_write.c
SOURCES += ../libc/libc_stub.c
SOURCES += ../libc/sbrk.cc

INCLUDES  = -I.
INCLUDES += -I../drivers
INCLUDES += -I../lib

OPTFLAG = -O3

include ../makefile-common.mk

