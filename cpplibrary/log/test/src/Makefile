#!/bin/sh

include ../../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../../include -I../../../../cppfoundation/include
CMPFLAG		= $(G_CMPFLAG) -c -pthread $(COMMINC)
LNKFLAG     = 
CFLIBDIR	= -L. -L../../bin -L../../../../cppfoundation/bin
CFLIB		= $(CFLIBDIR) -l$(G_STUBNAME) -l$(G_CLASSNAME) -l$(G_TARGETNAME) 
SYTMLIB     = -lpthread

EXE =   perf_log
OBJ = perf_log.o

BINDIR =../bin

all:$(EXE)

cleanobj:
	rm -rf $(OBJ)
cleanall:
	rm -rf $(OBJ)
	rm -rf $(BINDIR)/$(EXE)

%.o:%.cpp
	$(COMPER) $(CMPFLAG) $<

$(EXE):$(OBJ)
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(CFLIB) -o $@ $?
	mv $@ $(BINDIR)
