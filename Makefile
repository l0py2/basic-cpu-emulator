OUT := cpu auto-cpu char-display

CFLAGS := -Wall

all: $(OUT)

clean:
	$(RM) -f *.bin *.o $(OUT)

view-%: %.bin
	hexdump -x $<

run-%: %.bin cpu
	./cpu $<

cpu: cpu.o cpu-ui.o cpu-core.o
	$(CC) $^ `pkg-config --libs ncurses` -o $@

auto-cpu: auto-cpu.o cpu-core.o
	$(CC) $^ -o $@

char-display: char-display.o
	$(CC) $^ -o $@

cpu.o: cpu.c is.h cpu-ui.h cpu-core.h
auto-cpu.o: cpu.c is.h cpu-core.h
char-display.o: char-display.c is.h
cpu-ui.o: cpu-ui.c is.h
cpu-core.o: cpu-core.c is.h

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

%.bin: examples/%.s assembler
	./assembler $< $@

.NOTINTERMEDIATE: %.bin
.PHONY: all clean run run-example view-example
