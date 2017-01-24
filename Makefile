# gcc -L/usr/local/lib -I/usr/local/include/hidapi hidapitest.c -o hidapitest -lhidapi

CC=gcc
CFLAGS=-I/usr/local/include/hidapi -L/usr/local/lib -lhidapi
DEPS = 
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

respeaker_hid: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

