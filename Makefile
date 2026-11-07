EXAMPLE := basic
OUT := cpu

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

cpu.o: cpu.c is.h cpu-ui.h cpu-core.h
cpu-ui.o: cpu-ui.c is.h
cpu-core.o: cpu-core.c is.h

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

%.bin: examples/%.s
	./assembler $< $@

.NOTINTERMEDIATE: %.bin
.PHONY: all clean run run-example view-example
