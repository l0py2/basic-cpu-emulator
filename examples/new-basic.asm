# Test comment
start: # Test inline comment
	NOP
	LDI R1 0xfffe # Test pseudo instruction
	NOP
	LDHI R2 0xff
	LDLI R2 255
	NOP
	LDHI R8 0xff
	LDLI R8 0b01010101
loop:
	JUMP loop
