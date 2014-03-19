#!/bin/sh

include ../../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../../include -I../../../../cppfoundation/include
CMPFLAG		= $(G_CMPFLAG) -c -pthread $(COMMINC)
LNKFLAG     = 
LIBDIR	= -L. -L../../bin -L../../../../cppfoundation/bin
LIB		= $(LIBDIR) -l$(G_TARGETNAME) -l$(G_STUBNAME) -l$(G_CLASSNAME)
SYTMLIB     = -lpthread -lz

EXE0 =   test_server
EXE1 =   test_client
OBJ0 = test_server.o
OBJ1 = test_client.o

BINDIR =../bin

all:$(EXE0) $(EXE1)

cleanobj:
	rm -rf $(OBJ0)
	rm -rf $(OBJ1)
cleanall:
	rm -rf $(OBJ0)
	rm -rf $(OBJ1)
	rm -rf $(BINDIR)/$(EXE0)
	rm -rf $(BINDIR)/$(EXE1)

%.o:%.cpp
	$(COMPER) $(CMPFLAG) $<

$(EXE0):$(OBJ0)
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(LIB) -o $@ $?
	mv $@ $(BINDIR)

$(EXE1):$(OBJ1)
	$(LINKER) $(LNKFLAG) $(SYTMLIB) $(LIB) -o $@ $?
	mv $@ $(BINDIR)
    
