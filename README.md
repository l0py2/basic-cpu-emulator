# Basic CPU ~~Emulator~~

## Thanks to

- [https://docs.python.org/3/library/re.html#writing-a-tokenizer](https://docs.python.org/3/library/re.html#writing-a-tokenizer)

## Instructions

### Format

**\[opcode 5 bits\] \[register 3 bits\] \[value 8 bits\]**

#### Existing

| mnemonic | description                                   | result                            |
| -------- | --------------------------------------------- | --------------------------------- |
| NOP      | no operation                                  |                                   |
| LDHI     | load immediate to register high byte          | RX = imm << 8 \| (RX & 0x00ff)    |
| LDLI     | load immediate to register low byte           | RX = imm \| (RX & 0xff00)         |
| ADDI     | add immediate to register (signed)            | RX = RX + imm                     |
| RJUMP    | relative jump (signed)                        | PC = PC + imm                     |
| LDM      | load value from memory to register            | RY = mem\[RX\]                    |
| SVM      | save value to memory from register            | mem\[RX\] = RY                    |
| PUSH     | save value to satck from register             | mem\[SP\] = RX, SP = SP + 1       |
| POP      | load value from stack to register             | RX = mem\[SP\], SP = SP - 1       |
| RCALL    | relative call (signed)                        | mem\[SP\] = PC + 1, PC = PC + imm |
| RET      | return                                        | PC = mem\[SP\], SP = SP - 1       |
| RBREZ    | relative branch if register is equal to 0     | RX == 0 ? PC = PC + imm           |
| RBRNZ    | relative branch if register is not equal to 0 | RX == 0 ? PC = PC + imm           |

### Other (planned)

| mnemonic | description  | result |
| -------- | ------------ | ------ |
| NOP      | no operation |        |

### Basic (planned)

| mnemonic | description                          | result                         |
| -------- | ------------------------------------ | ------------------------------ |
| LDHI     | load immediate to register high byte | RX = imm << 8 \| (RX & 0x00ff) |
| LDLI     | load immediate to register low byte  | RX = imm \| (RX & 0xff00)      |

### Arithmetic (planned)

| mnemonic | description           | result       |
| -------- | --------------------- | ------------ |
| ADD      | add registers         | RX = RY + RZ |
| SUB      | subtract registers    | RX = RY - RZ |
| NEG      | unary negate register | RX = -RY     |
| MULT     | multiply registers    | RX = RY * RZ |
| DIV      | divide registers      | RX = RY / RZ |
| REM      | registers remainder   | RX = RY % RZ |

#### Bitwise (planned)

| mnemonic | description | result         |
| -------- | ----------- | -------------- |
| NOT      |             | RX = ~RY       |
| AND      |             | RX = RY & RZ   |
| OR       |             | RX = RY \| RZ  |
| XOR      |             | RX = RY ^ RZ   |
| SL       |             | RX = RY << RZ  |
| SR       |             | RX = RY >> RZ  |

### Branching (planned)

| mnemonic | description | result             |
| -------- | ----------- | ------------------ |
| JUMP     |             | PC = RX            |
| BREQ     |             | RY == RZ ? PC = RX |
| BRNE     |             | RY != RZ ? PC = RX |
| BRGT     |             | RY > RZ ? PC = RX  |
| BRLT     |             | RY < RZ ? PC = RX  |

### Memory (planned)

| mnemonic | description | result         |
| -------- | ----------- | -------------- |
| LDM      |             | RX = mem\[RY\] |
| SVM      |             | mem\[RY\] = RX |

### Stack (planned)

| mnemonic | description | result                                   |
| -------- | ----------- | ---------------------------------------- |
| PUSH     |             | mem\[SP\] = RX, SP = SP - 1              |
| POP      |             | SP = SP + 1, RX = mem\[SP\]              |
| CALL     |             | mem\[SP\] = PC + 1, SP = SP - 1, PC = RX |
| RET      |             | SP = SP + 1, PC = mem\[SP\]              |

## Pseudo instructions

| mnemonic | description                | real instructions |
| -------- | -------------------------- | ----------------- |
| LDI      | load immediate to register | LDHI + LDLI       |

## Assembler directives

| mnemonic | description                              |
| -------- | ---------------------------------------- |
| .DEF     | define a symbol                          |
| .RESERVE | create a block of memory filled with NOP |

## Registers

- R1
- R2
- R3
- R4
- R5
- R6
- R7
- R8
