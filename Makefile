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

cpu: cpu.o
	$(CC) $^ `pkg-config --libs ncurses` -o $@

new-cpu.o: new-cpu.c is.h

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

%.bin: examples/%.s
	./assembler $< $@

.NOTINTERMEDIATE: %.bin
.PHONY: all clean run run-example view-example
