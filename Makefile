OUT := cpu

all: cpu

clean:
	${RM} -f *.o ${OUT}

cpu: cpu.o example-program.o
	${CC} $^ -o $@

cpu.o: cpu.c example-program.h is.h
example-program.o: example-program.c example-program.h is.h

%.o: %.c
	${CC} $< -c -o $@

.PHONY: all
