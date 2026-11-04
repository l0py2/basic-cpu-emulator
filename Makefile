EXAMPLE := basic
OUT := cpu debug

CFLAGS := -Wall

all: $(OUT)

clean:
	$(RM) -f *.bin *.o $(OUT)

run-example: cpu out.bin
	./cpu out.bin

view-example: out.bin
	hexdump -x out.bin

out.bin: assembler examples/$(EXAMPLE).s
	./assembler examples/$(EXAMPLE).s out.bin

cpu: cpu.o
debug: debug.o
	$(CC) $^ `pkg-config --libs ncurses` -o $@

cpu.o: cpu.c is.h
debug.o: debug.c is.h

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

%: %.o
	$(CC) $^ -o $@

.PHONY: all clean run run-example view-example
