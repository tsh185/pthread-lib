CC = cc
LIBS = -lpthread
FLAGS = -O -w
EXECS = test

TEST_OBJS = test.o multi_thread.o

all: $(EXECS)

multi_thread.o: multi_thread.c multi_thread.h

test: $(TEST_OBJS)
	$(CC) $(FLAGS) -o $@ $? $(LIBS)

clean:
	rm $(EXECS)
