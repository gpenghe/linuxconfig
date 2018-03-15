PROGS = pollsvr$(EXT) pollsvr_thread$(EXT)
CFLAGS=-Wall -Wextra

ifdef arm
CC = arm-none-linux-gnueabi-gcc
EXT = .arm
else
CC = gcc
EXT = .pc
CFLAGS += -m32
endif

all: $(PROGS)

pollsvr$(EXT): pollsvr.c
	$(CC) $(CFLAGS) -o $@ $^

pollsvr_thread$(EXT): pollsvr_thread.c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

clean:
	-rm -f *.o $(PROGS)
