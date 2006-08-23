# Example Makefile using Makefile.inc

# include Makefile.inc, use a relative pathname
include Makefile.inc

SUBDIRS=

# list executables here
EXE_LIST = \
	test \
	test_queue \
	example1 \
        example2 \
        example3 \
	test_task

# list object file dependencies for each executable here
MULTI_THREAD_OBJS = \
	multi_thread.o

TEST_OBJS = \
	test.o \
	multi_thread.o

TEST_QUEUE_OBJS = \
	test_queue.o \
	gen_queue.o

TEST_TASK_OBJS = \
	test_task.o \
	task.o

EXAMPLE1_OBJS = \
	example1.o \
	multi_thread.o

EXAMPLE2_OBJS = \
	example2.o \
	multi_thread.o

EXAMPLE3_OBJS = \
	example3.o \
	multi_thread.o

# list all Pro*C source files here
PROC_LIST =

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

test_task.o: test_task.c task.h

task.o: task.c task.h

gen_queue.o: gen_queue.c gen_queue.h

example1.o: example1.c

example2.o: example2.c

example3.o: example3.c

# link targets (one for each executable)
test: $(TEST_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(TEST_OBJS) -o $@ \
	    -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

test_queue: $(TEST_QUEUE_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(TEST_QUEUE_OBJS) -o $@ \
	    -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

test_task: $(TEST_TASK_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(TEST_TASK_OBJS) -o $@ \
	    -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

example1: $(EXAMPLE1_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(EXAMPLE1_OBJS) -o $@ \
	    -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

example2: $(EXAMPLE2_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(EXAMPLE2_OBJS) -o $@ \
	    -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

example3: $(EXAMPLE3_OBJS)
	@$(ECHO) "\n*** Target: $@ (linking)"
	$(CC) $(LINK_FLAGS) $(CINCL) $(EXAMPLE3_OBJS) -o $@ \
	    -l$(LIBPTHREAD)
	@$(CHMOD) 750 $@

# target to clean build environment (do not modify)
clean:
	@$(ECHO) "\n*** Target: $@"
	$(RM) $(EXE_LIST) $(PROC_LIST:.pc=.c) *.o *.lis 
