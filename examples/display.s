display:
	.RESERVE 100 # 10x10 display

start:
	LDI R1 0 # pixel = 0
	LDI R4 1
loop_start:
	LDI R2 display
	LDI R3 100
	ADD R3 R3 R2
loop:
	SVM R1 R2

	ADD R2 R2 R4 # display++
loop_check:
	LDI R8 loop
	BRNE R8 R2 R3
	ADD R1 R1 R4 # pixel++
	LDI R8 loop_start
	JUMP R8
