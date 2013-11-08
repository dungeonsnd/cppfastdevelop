#!/bin/sh

include ../../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../../include
CMPFLAG		= $(G_CMPFLAG) -c -fPIC $(COMMINC)
LNKFLAG     = -shared 
SYTMLIB     = -lrt -ldl 

DLL =lib$(G_STUBNAME).so
OBJ = cf_system_stub.o
BINDIR =../../bin

all:$(DLL)

cleanobj:
	rm -rf $(OBJ)
cleanall:
	rm -rf $(OBJ)
	rm -rf $(BINDIR)/$(DLL)

%.o:%.c
	$(COMPER) $(CMPFLAG) $<

$(DLL):$(OBJ)
	$(LINKER) $(LNKFLAG) $(SYTMLIB)  -o $@ $?
	mv $@ $(BINDIR)
