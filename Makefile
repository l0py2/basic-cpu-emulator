OUT := cpu assembler

all: cpu assembler

clean:
	${RM} -f *.bin *.o ${OUT}

cpu: cpu.o example-program.o
assembler: assembler.o

cpu.o: cpu.c example-program.h is.h
example-program.o: example-program.c example-program.h is.h
assembler.o: assembler.c

%.o: %.c
	${CC} $< -c -o $@

%: %.o
	${CC} $^ -o $@

.PHONY: all clean
