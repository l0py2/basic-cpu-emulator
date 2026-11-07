char:
	.RESERVE 1

start:
	LDI R1 char
loop:
	RJUMP loop
