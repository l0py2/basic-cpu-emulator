# Test directives
.DEF test_value 0xf0f0
.RESERVE 4

# Test comment
start: # Test inline comment
	NOP
	LDI R1 0xfffe # Test pseudo instruction
	NOP
	LDHI R2 0xff
	LDLI R2 255
	NOP
	LDI R3 0xffff
	ADDI R3 -1 # Test signed addition
	NOP
	LDI R4 -1 # Test signed load
	ADDI R4 2
	NOP
	LDI R5 test_value
	NOP
	LDHI R8 0xff
	LDLI R8 0b01010101
	RJUMP loop
	NOP
	NOP
	NOP
loop:
	RJUMP loop
