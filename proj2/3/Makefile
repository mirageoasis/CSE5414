CC = gcc
LDFLAGS=-lm -lpthread -lrt

all: seq_reverse seq_gray seq_smooth

seq_reverse:
	$(CC) -g -o ./serial/seq_reverse ./serial/serial_reverse.c
seq_gray:
	$(CC) -g -o ./serial/seq_gray ./serial/serial_gray.c
seq_smooth:
	$(CC) -g -o ./serial/seq_smooth ./serial/serial_smooth.c
seq_go:
	./serial/seq_reverse
	./serial/seq_gray
	./serial/seq_smooth

clean:
	rm -rf *~ ./serial/seq_reverse ./serial/seq_gray ./serial/seq_smooth *.o *.out