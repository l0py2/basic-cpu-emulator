display:
	.RESERVE 100 # 10x10 display

start:
	LDI R1 display # display pointer
	LDI R2 0       # pixel

	loop:
		SVM R1 R2
		ADDI R1 1
		ADDI R2 1
		RJUMP loop
