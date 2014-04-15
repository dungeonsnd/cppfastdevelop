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

EXE1 =   perf_log
EXE2 =   perf_log_cache
EXE  =   $(EXE1) $(EXE2)

OBJ1 = perf_log.o
OBJ2 = perf_log_cache.o
OBJ  = $(OBJ1) $(OBJ2)

BINDIR =../bin

all:$(EXE)

cleanobj:
	rm -rf $(OBJ)
cleanall:
	rm -rf $(OBJ)
	rm -rf $(BINDIR)/$(EXE)

%.o:%.cpp
	$(COMPER) $(CMPFLAG) $<

$(EXE1):$(OBJ1)
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(CFLIB) -o $@ $?
	mv $@ $(BINDIR)
    
$(EXE2):$(OBJ2)
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(CFLIB) -o $@ $?
	mv $@ $(BINDIR)

