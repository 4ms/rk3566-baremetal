SOURCES = startup.s 
SOURCES += main.cc
SOURCES += mmu.cc

SOURCES += ../drivers/init.cc
SOURCES += ../drivers/serial.cc
SOURCES += ../drivers/print.cc
SOURCES += ../system/libc/read_write.c
SOURCES += ../system/libc/libc_stub.c
SOURCES += ../system/libc/sbrk.cc

INCLUDES  = -I.
INCLUDES += -I..
INCLUDES += -I../lib

OPTFLAG = -O3

INCLUDES += -I../drivers/mdrivlib/target/aarch64

include ../makefile-common.mk

