setup:
	LDLI R1 1
	NOP
	RCALL test
	LDLI R1 3
	NOP
loop:
	ADDI R1 1
	RJUMP loop

test:
	LDLI R1 2
	NOP
	RET
