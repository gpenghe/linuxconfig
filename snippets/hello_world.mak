PROG = test

CFLAGS += -Wall 

$(PROG): test.o
	gcc $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	-rm -rf *.o $(PROG)
