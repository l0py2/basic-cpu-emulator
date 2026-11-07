char:
	.RESERVE 1

start:
	LDI R1 char

	LDI R2 65
	SVM R1 R2

	LDI R2 66
	SVM R1 R2

	LDI R2 67
	SVM R1 R2
loop:
	RJUMP loop
