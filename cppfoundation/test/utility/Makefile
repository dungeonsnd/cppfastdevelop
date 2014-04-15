#!/bin/sh

include ../../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../../include
CMPFLAG		= $(G_CMPFLAG) -c $(COMMINC)
LNKFLAG     =
CFLIBDIR	= -L. -L../../bin
CFLIB		= $(CFLIBDIR) -l$(G_CLASSNAME) -l$(G_STUBNAME)
SYTMLIB     =

BINDIR =../bin

all: utility_program_test 

cleanobj:
	rm -rf utility_program_test.o
cleanall:
	rm -rf $(OBJ)
	rm -rf $(BINDIR)/utility_program_test

%.o:%.cpp
	$(COMPER) $(CMPFLAG) $<

utility_program_test:utility_program_test.o 
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(CFLIB) -o $@ $?
	mv $@ $(BINDIR)
