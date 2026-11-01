EXAMPLE := basic
OUT := cpu debug vga-text

CFLAGS := -Wall

all: $(OUT)

clean:
	$(RM) -f *.bin *.o $(OUT)

run-example: cpu out.bin
	./cpu out.bin

view-example: out.bin
	hexdump -x out.bin

cpu: cpu.o
debug: debug.o
	$(CC) $^ `pkg-config --libs ncurses` -o $@
vga-text: vga-text.o

cpu.o: cpu.c is.h
debug.o: debug.c is.h
vga-text.o: vga-text.c is.h

out.bin: assembler examples/$(EXAMPLE).asm
	./assembler examples/$(EXAMPLE).asm out.bin

%.o: %.c
	$(CC) $< -c -o $@

%: %.o
	$(CC) $^ -o $@

.PHONY: all clean run run-example view-example
