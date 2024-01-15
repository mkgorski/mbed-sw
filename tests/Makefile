CC  = gcc
SRC_DIR := ../src

SRCS  = main.c fifo.c fifo_tests.c \
				slist.c slist_tests.c \
				dlist.c dlist_tests.c \
				alloc.c alloc_tests.c \
				alloc2.c alloc2_tests.c \
				timer.c timer_tests.c \
				stringstream.c stringstream_tests.c
OBJS = $(SRCS:.c=.o)

CFLAGS   = -W -Wall -O0 -g3 --std=gnu99 -fgnu89-inline -include hw.h
CFLAGS  += -I. -I"$(SRC_DIR)" -I"$(SRC_DIR)/core"


VPATH = $(SRC_DIR)/core

.PHONY: all tests clean

all: clean tests
	./tests

tests: $(OBJS) 
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	@$(CC) $(CFLAGS) -MM $< -MF $*.d -MP -MT "$@"
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *.d tests
