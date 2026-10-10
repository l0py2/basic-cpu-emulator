# Basic CPU ~~Emulator~~

## Instructions

### Format

**\[opcode 5 bits\] \[register 3 bits\] \[value 8 bits\]**

### Opcodes

- NOP (no operation)

- LDHI (load imediate to register high byte)
- LDLI (load imediate to register low byte)
- LDM (load memory to register)
- SVM (save register to memory)
- COPY (copy from a register to another register)

- ADDI (add imediate to register)
- SUBI (subtract imediate to register)
- ADD (add register to another register)
- SUB (subtract register to another register)
- MULT (multiply a register with another register)
- DIV (divide a register with another register)
- MOD (modulus of register with another register)

- COMPI (compare imediate to register \[register - imediate value\])
- COMP (compare register to another register)

- SLI (shift register to the left by imediate)
- SRI (shift register to the right by imediate)
- SET (sets all bits of register)
- CLR (clear all bits of register)
- NOT (bitwise NOT on a register)
- AND (bitwise AND between a register and another register)
- OR (bitwise OR between a register and another register)

- JUMP (jump to address)
- BRNEQ (jump to address if ZERO != 0)
- BREQ (jump to address if ZERO == 0)
- CALL (save next address to stack and jump to address)

- PUSH (save value from register to stack)
- POP (load value from stack to register)
- RET (jump to address from stack)

### Registers

- R1
- R2
- R3
- R4
- R5
- R6
- R7
- R8/FLAGS

### Flags

- ZERO
- OVERFLOW

## Assembler

```as
NOP 0 0

Example_label1:
    CLR R1 0
    LDLI R1 0x10

JUMP 0 Example_label1
```
