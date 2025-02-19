SOURCES = startup.s 
SOURCES += main.cc
SOURCES += mmu.cc

SOURCES += ../drivers/init.cc
SOURCES += ../drivers/serial.cc
SOURCES += ../drivers/print.cc

INCLUDES  = -I.
INCLUDES += -I../drivers
INCLUDES += -I../lib

INCLUDES += -I../drivers/mdrivlib/target/rk3566
INCLUDES += -I../drivers/mdrivlib/target/aarch64

OPTFLAG = -O3

include ../makefile-common.mk

