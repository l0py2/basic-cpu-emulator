display:
	.RESERVE 100 # 10x10 display

start:
	LDI R3 0 # pixel

	loop_setup:
		LDI R1 display # display pointer
		LDI R2 100     # i
	loop:
		SVM R1 R3
		ADDI R3 1

		ADDI R1 1
		ADDI R2 -1

		RBREZ R2 loop_setup
		RJUMP loop
