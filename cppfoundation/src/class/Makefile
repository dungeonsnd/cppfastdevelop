#!/bin/sh

include ../../global.mk

COMPER		= $(G_COMPER)
LINKER		= $(G_LINKER)
COMMINC		= -I. -I../../include
CMPFLAG		= $(G_CMPFLAG) -c -fPIC -pthread $(COMMINC)
LNKFLAG     = -shared 
CFLIBDIR	= -L. -L../../bin
CFLIB		= $(CFLIBDIR) -l$(G_STUBNAME)
SYTMLIB     = -lpthread

DLL =   lib$(G_CLASSNAME).so
OBJ =   \
cf_exception.o \
cf_lock.o \
cf_condition.o \
cf_ipc.o  \
cf_thread.o \
cf_binary_search.o \
cf_utility.o \
cf_memory.o \
cf_concurrency_queue.o \
cf_concurrency_map.o \
cf_concurrency_sequence_container.o  \
cf_concurrency_set.o \
cf_process.o \
cf_application.o \
cf_network.o \
cf_socket.o \
cf_event_loop.o \
cf_epoll.o\
cf_kqueue.o\
cf_poll.o\
cf_crypto.o

BINDIR =../../bin

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
