# PThreads
LIBPTHREAD=pthread
PTHREADCFLAGS=-D_POSIX_C_SOURCE=199506L -D_HPUX_SOURCE

# C Compiler
CC=/usr/bin/cc
CFLAGS=-g -O -Ae -DHPUX $(PTHREADCFLAGS)
CINCL=
LINK_FLAGS=-g
DEBUG=-DDEBUG 

# fully-qualified UNIX commands
RM=/usr/bin/rm -f
CHMOD=/usr/bin/chmod
ECHO=/usr/bin/echo

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
