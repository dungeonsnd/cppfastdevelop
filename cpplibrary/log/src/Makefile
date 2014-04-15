#!/bin/sh

include ../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../include -I../../../cppfoundation/include
CMPFLAG		= $(G_CMPFLAG) -c -fPIC -pthread $(COMMINC)
LNKFLAG     = -shared
CFLIBDIR	= -L. -L../../../cppfoundation/bin
CFLIB		= $(CFLIBDIR) -l$(G_CLASSNAME)
SYTMLIB     = -lpthread

DLL =   lib$(G_TARGETNAME).so
OBJ = cl_log.o cl_log_cache.o

BINDIR =../bin

all:$(DLL)

cleanobj:
	rm -rf $(OBJ)
cleanall:
	rm -rf $(OBJ)
	rm -rf $(BINDIR)/$(DLL)

%.o:%.cpp
	$(COMPER) $(CMPFLAG) $<

$(DLL):$(OBJ)
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(CFLIB) -o $@ $?
	mv $@ $(BINDIR)
