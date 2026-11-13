#!/usr/bin/env python3

from itertools import tee
from ctypes import c_uint16, c_int8
from sys import argv

from tokenizer import tokenize
from generator import generate_symbol_table, generate_program

def read_file(path):
    with open(path, 'r') as file:
        return file.read()

    return ''

def write_program_file(path, program):
    with open(path, 'wb') as file:
        for value in program:
            file.write(c_uint16(value))

if len(argv) < 3:
    print(f'Usage: {argv[0]} [input path] [output path]')
    exit(1)

# name: value
instructions = {
    'NOP':    0 << 11,
    'LDHI':   1 << 11,
    'LDLI':   2 << 11,
    'ADDI':   3 << 11,
    'RJUMP':  4 << 11,
    'LDM':    5 << 11,
    'SVM':    6 << 11,
    'PUSH':   7 << 11,
    'POP':    8 << 11,
    'RCALL':  9 << 11,
    'RET':   10 << 11,
    'RBREZ': 11 << 11,
    'RBRNZ': 12 << 11
}

# name: instruction count
pseudo_instructions = {
    'LDI': 2
}

# name: value
registers = {
    'R1': 0,
    'R2': 1,
    'R3': 2,
    'R4': 3,
    'R5': 4,
    'R6': 5,
    'R7': 6,
    'R8': 7
}

src = read_file(argv[1])

symbol_tokens, program_tokens = tee(
    tokenize(src, instructions, pseudo_instructions, registers),
    2
)

symbol_table = generate_symbol_table(symbol_tokens, pseudo_instructions)
program = generate_program(program_tokens, symbol_table, instructions, pseudo_instructions, registers)

write_program_file(argv[2], program)

print('=== Symbol Table ===')
for symbol, value in symbol_table.items():
    print(f'{symbol} = {value}')

print('=== Program Memory ===')
for i in range(len(program)):
    print(f'0x{format(i, '04x')}: 0x{format(program[i], '04x')} = 0b{format(program[i], '016b')}')
