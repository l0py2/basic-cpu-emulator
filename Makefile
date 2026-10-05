OUT := cpu assembler
EXAMPLE := basic

all: $(OUT)

clean:
	$(RM) -f *.bin *.o $(OUT)

run-example: cpu out.bin
	./cpu out.bin

view-example: out.bin
	hexdump -x out.bin

cpu: cpu.o tui.o
	$(CC) $^ `pkg-config --libs ncurses` -o $@

assembler: assembler.o

cpu.o: cpu.c is.h tui.h
tui.o: tui.c tui.h
assembler.o: assembler.c is.h

out.bin: assembler examples/$(EXAMPLE).scasm
	./assembler examples/$(EXAMPLE).scasm out.bin

%.o: %.c
	$(CC) $< -c -o $@

%: %.o
	$(CC) $^ -o $@

.PHONY: all clean run run-example view-example
