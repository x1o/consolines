CFLAGS += -std=c99 -Wall
LDLIBS += -lm -lncurses

all: lines

clean:
	rm -f *.o lines

lines.o: lines.c
	gcc $(CFLAGS) -c $^ -o $@

main.o: main.c
	gcc $(CFLAGS) -c $^ -o $@

lines: lines.o main.o
	gcc $^ $(LDLIBS) -o $@
