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
