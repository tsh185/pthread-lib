# PThreads
LIBPTHREAD=pthread
LIBGDSL=-L/usr/local/lib -lgdsl
LIBEXPAT=libexpat
LIBSCEW=libscew

PTHREADCFLAGS=
blah=-D_POSIX_C_SOURCE=199506L -D_HPUX_SOURCE

# C Compiler
CC=cc
CFLAGS=-g -O
CINCL=-I/usr/local/include/gdsl
LINK_FLAGS=-g
DEBUG=-DDEBUG 

# commands
RM=rm -f
CHMOD=chmod
ECHO=echo

#
# Suffix rules
#
.SUFFIXES:	.c .o

.c.o:
	@$(ECHO)
	@$(ECHO) "\n*** Target: $@ (compilation)"
	$(CC) $(CFLAGS) $(CINCL) $(DEBUG) -c $*.c

.pc.c:
	@$(ECHO) "\n*** Target: $@ (precompilation)"
	$(PROC) $(PROC_FLAGS) $(PROC_INCL) iname=$* lines=yes \
	    code=ansi_c unsafe_null=yes define=_PA_RISC1_1 \
	@$(CHMOD) 640 $*.c
