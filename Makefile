CFLAGS += -std=c99 -Wall
LDLIBS += -lm -lncurses

lines: lines.c
	gcc $(CFLAGS) $^ $(LDLIBS) -o $@
