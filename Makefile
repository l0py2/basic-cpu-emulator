OUT := cpu

all: cpu

clean:
	${RM} -f *.o ${OUT}

cpu: cpu.o
	${CC} $^ -o $@

%.o: %.c
	${CC} $< -c -o $@

.PHONY: all
