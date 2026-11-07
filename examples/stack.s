setup:
	LDI R1 1
	NOP
	RCALL test
	LDI R1 3
	NOP
loop:
	RJUMP loop

test:
	LDI R1 2
	NOP
	RET
