#!/bin/sh

include ../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../include -I../../../cppfoundation/include
CMPFLAG		= -g -O0 -c -fPIC -Wall $(COMMINC)
LNKFLAG     = -shared
CFLIBDIR	= -L. -L../../../cppfoundation/bin
CFLIB		= $(CFLIBDIR) -l$(G_CLASSNAME)
SYTMLIB     = 


DLL =lib$(G_TARGETNAME).so
OBJ = cl_circular_queue.o cl_queue_operation.o
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
