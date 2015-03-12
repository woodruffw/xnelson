CC = cc
CFLAGS = -std=c99 -O3
LDFLAGS = -lX11
CPPFLAGS = -D_POSIX_C_SOURCE=200112L

SOURCES = $(wildcard ./*.c)
OBJECTS = $(SOURCES:.c=.o)

PROG = xnelson

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJECTS) -o $(PROG) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f *.o $(PROG)

.PHONY: all clean
