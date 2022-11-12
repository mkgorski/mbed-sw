CC  = gcc
SRC_DIR := ../src

SRCS  = main.c fifo.c fifo_tests.c slist.c slist_tests.c
OBJS = $(SRCS:.c=.o)

CFLAGS   = -W -Wall -O0 -g3 --std=gnu99 -fgnu89-inline
CFLAGS  += -I. -I"$(SRC_DIR)" -I"$(SRC_DIR)/fifo" -I"$(SRC_DIR)/slist"


VPATH = $(SRC_DIR)/fifo:$(SRC_DIR)/slist

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
