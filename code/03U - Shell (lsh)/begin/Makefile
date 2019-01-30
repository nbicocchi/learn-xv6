CC=gcc
CFLAGS=-O2 -Wall
BINS=lsh utils

all: $(BINS)
	

clean:
	rm -rf *.o $(BINS)


%: %.c
	$(CC) $< $(CFLAGS) -o $@