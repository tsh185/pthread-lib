# Example Makefile using Makefile.inc

# include Makefile.inc, use a relative pathname
include Makefile.inc

SUBDIRS=

# list executables here
EXE_LIST =\
	in_closer \
	count_currval \
	conf_resv \
        thread \
        rp


# list object file dependencies for each executable here

# object files for main1
IN_CLOSER_OBJS =\
        in_closer.o \
	thread_gen.o 

# object files for main2
COUNT_CURRVAL_OBJS =\
        count_currval.o \
	thread_gen.o 

# object file for conf_resv
CONF_RESV_OBJS =\
	conf_resv.o \
	thread_gen.o

THREAD_OBJS = \
        thread.o \
        thread_gen.o

RP_OBJS = \
        rp.o \
        thread_gen.o

MULTI_THREAD_OBJS = \
	multi_thread.o

TEST_OBJS = \
	test.o \
	multi_thread.o

TEST_QUEUE_OBJS = \
	test_queue.o \
	gen_queue.o

# list all Pro*C source files here
PROC_LIST =\
	in_closer.pc \
	count_currval.pc \
	conf_resv.pc \
	thread_gen.pc \
        thread.pc \
        rp.pc \
	multi_thread.c \
	test.c

# list all non-Pro*C source files here
NONPROC_LIST =\
	thread_gen.h \
	structs_data.h \
        rp.h \
	multi_thread.h \

# target to build all subdirectories (do not modify)
all: $(EXE_LIST)
	@$(ECHO) "\n*** Target: $@"
	@if [ "$(SUBDIRS)X" != "X" ]; then		\
	    SUBDIRS=$(SUBDIRS);				\
	    for subdir in $$SUBDIRS; do			\
	        $(ECHO) "making in $$subdir";		\
	        ( cd $$subdir && make $@ ) || exit 1;	\
	    done					\
	fi

# CVS update target for source files
$(NONPROC_LIST) $(PROC_LIST):
	cvs update $@;


# dependencies for each object file 

in_closer.o: in_closer.c structs_data.h thread_gen.h

count_currval.o: count_currval.c thread_gen.h structs_data.h

conf_resv.o: conf_resv.c thread_gen.h structs_data.h

thread_gen.o: thread_gen.c thread_gen.h

thread.o: thread.c 

rp.o: rp.c 

multi_thread.o: multi_thread.c multi_thread.h util.h

test.o: test.c

test_queue.o: test_queue.c

gen_queue.o: gen_queue.c gen_queue.h

#rp.o: rp.c thread_gen.h 

# link targets (one for each executable)

# link main1
in_closer: $(IN_CLOSER_OBJS)	
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(IN_CLOSER_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

# link main2
count_currval: $(COUNT_CURRVAL_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(COUNT_CURRVAL_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

# link conf_resv
conf_resv: $(CONF_RESV_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(CONF_RESV_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@


thread: $(THREAD_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(THREAD_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

rp: $(RP_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(RP_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

test: $(TEST_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(TEST_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

test_queue: $(TEST_QUEUE_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(TEST_QUEUE_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

# target to clean build environment (do not modify)
clean:
	@$(ECHO) "\n*** Target: $@"
	$(RM) $(EXE_LIST) $(PROC_LIST:.pc=.c) *.o *.lis 
