# Example Makefile using Makefile.inc

# include Makefile.inc, use a relative pathname
include Makefile.inc

SUBDIRS=

# list executables here
EXE_LIST = \
	test \
	test_queue \
	example

# list object file dependencies for each executable here
MULTI_THREAD_OBJS = \
	multi_thread.o

TEST_OBJS = \
	test.o \
	multi_thread.o

TEST_QUEUE_OBJS = \
	test_queue.o \
	gen_queue.o

EXAMPLE_OBJS = \
	example.o \
	multi_thread.o

# list all Pro*C source files here
PROC_LIST =\
	multi_thread.c \
	test.c \
    example.c

# list all non-Pro*C source files here
NONPROC_LIST =\
	multi_thread.h

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


# dependencies for each object file 
multi_thread.o: multi_thread.c multi_thread.h util.h

test.o: test.c

test_queue.o: test_queue.c

gen_queue.o: gen_queue.c gen_queue.h

example.o: example.c

# link targets (one for each executable)
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

example: $(EXAMPLE_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(EXAMPLE_OBJS) -o $@ \
	    -L$(CGTOLIBDIR) -L$(MQMLIBDIR) -L$(ORALIBDIR) \
	    -l$(LIBCGTO) -l$(LIBMQM) -l$(LIBORA) -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

# target to clean build environment (do not modify)
clean:
	@$(ECHO) "\n*** Target: $@"
	$(RM) $(EXE_LIST) $(PROC_LIST:.pc=.c) *.o *.lis 
