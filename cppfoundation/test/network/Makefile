#!/bin/sh

include ../../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../../include
CMPFLAG		= $(G_CMPFLAG) -c $(COMMINC)
LNKFLAG     =
CFLIBDIR	= -L. -L../../bin
CFLIB		= $(CFLIBDIR) -l$(G_STUBNAME)
SYTMLIB     =


BINDIR =../bin

all: network_test 

cleanobj:
	rm -rf network_test.o
cleanall:
	rm -rf $(OBJ)
	rm -rf $(BINDIR)/network_test

%.o:%.cpp
	$(COMPER) $(CMPFLAG) $<

network_test:network_test.o 
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(CFLIB) -o $@ $?
	mv $@ $(BINDIR)
