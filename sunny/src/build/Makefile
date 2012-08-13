# Makefile 

all:

PROG = sunny
PROG_NAME = avi_product

VPATH = .. 
MKRULES_DIR = ../../mk
PROJECTS_DIR = ../..

LDADD += -lpthread
LDADD += -ljthread
LDADD += -lrt


LDFLAGS += -L$(PROJECTS_DIR)/lib/jthread

CFLAGS += -I.
CFLAGS += -I..
CFLAGS += -I$(PROJECTS_DIR)/include/jthread
CFLAGS += -I$(PROJECTS_DIR)/include/utils


OBJS = main.o 
OBJS += watchdog.o
OBJS += interface_i2c.o
OBJS += fifoutil.o
OBJS += message.o
OBJS += input.o
OBJS += player.o

include $(MKRULES_DIR)/Rules.make

	
