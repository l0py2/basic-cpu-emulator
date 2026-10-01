# Basic CPU ~~Emulator~~

## Instrcutions

### Format

\[opecode 4 bits\] \[register 2 bits\] \[value 8 bits\]

### Opcodes

- 0000 NOP (no operation)
- 0001 LDI (load imediate to register)
- 0010 ADI (add imediate to register)
- 0011 JMP (jump to address)
- 0100 LDR (load value from memory to register)
- 0101 SVR (save value from register to memory)
- 0110 SBI (subtract imeditate from register)
- 0111 SLI (shift register to the left by imediate)
- 1000 SRI (shift register to the right by imediate)
- 1001 CAL (save next address to stack and jump to address)
- 1010 RET (jump to address saved in stack)
- 1011 ADD (add register to another register)
- 1100 SUB (subtract register from register)
- 1101 PSH (save value from register to stack)
- 1110 POP (load value from stack to register)
