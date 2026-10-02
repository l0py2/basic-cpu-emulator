OUT := cpu assembler

all: cpu assembler

clean:
	${RM} -f *.bin *.o ${OUT}

run:
	./cpu

run-example: cpu example.bin
	./cpu example.bin

example: example.bin
	hexdump -x example.bin

example.bin: assembler example.scasm
	./assembler example.scasm example.bin

cpu: cpu.o example-program.o
assembler: assembler.o

cpu.o: cpu.c example-program.h is.h
example-program.o: example-program.c example-program.h is.h
assembler.o: assembler.c

%.o: %.c
	${CC} $< -c -o $@

%: %.o
	${CC} $^ -o $@

.PHONY: all clean run run-example example
