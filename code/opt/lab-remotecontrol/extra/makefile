CC=gcc
CFLAGS=-Wall -I../utils/
BINS=xv6_monitor

all: $(BINS)
	
clean:
	rm -rf *.o $(BINS)

%: %.c $(HEADERS) $(LIBS)
	$(CC) $(CFLAGS) $(LIBS) $< -o $@
