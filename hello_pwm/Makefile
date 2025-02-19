SOURCES = startup.s 
SOURCES += main.cc
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

# ignore this static assert failure.
include ../makefile-common.mk
