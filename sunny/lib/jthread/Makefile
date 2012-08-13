# Makefile for jthread library 

LIB = libjthread.so 

PROJECTS_DIR = ../..
MKRULES_DIR  = $(PROJECTS_DIR)/mk
CFLAGS += -I$(PROJECTS_DIR)/include/control
CFLAGS += -I$(PROJECTS_DIR)/include/jthread

OBJS += jmutex.o 
OBJS += jthread.o

include $(MKRULES_DIR)/Rules.make
	
