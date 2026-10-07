EXAMPLE := basic
OUT := cpu assembler debug

CFLAGS := -Wall

all: $(OUT)

clean:
	$(RM) -f *.bin *.o $(OUT)

run-example: cpu out.bin
	./cpu out.bin

view-example: out.bin
	hexdump -x out.bin

cpu: cpu.o
assembler: assembler.o
debug: debug.o
	$(CC) $^ `pkg-config --libs ncurses` -o $@

cpu.o: cpu.c is.h
assembler.o: assembler.c is.h
debug.o: debug.c is.h

out.bin: assembler examples/$(EXAMPLE).scasm
	./assembler examples/$(EXAMPLE).scasm out.bin

%.o: %.c
	$(CC) $< -c -o $@

%: %.o
	$(CC) $^ -o $@

.PHONY: all clean run run-example view-example
