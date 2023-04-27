CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm -ggdb3 -g
FILES= project.c

.PHONY: hinfosvc clean

all:hinfosvc

hinfosvc: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	@rm -f hinfosvc