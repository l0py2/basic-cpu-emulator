#!/usr/bin/env python3

from itertools import tee
from ctypes import c_uint16, c_int8
from sys import argv

from tokenizer import tokenize
from generator import Generator

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
	'NOP' : 0b00000 << 11,
	'LDHI': 0b00001 << 11,
	'LDLI': 0b00010 << 11,
	'ADD' : 0b00011 << 11,
	'SUB' : 0b00100 << 11,
	'NEG' : 0b00101 << 11,
	'MULT': 0b00110 << 11,
	'DIV' : 0b00111 << 11,
	'REM' : 0b01000 << 11,
	'NOT' : 0b01001 << 11,
	'AND' : 0b01010 << 11,
	'OR'  : 0b01011 << 11,
	'XOR' : 0b01100 << 11,
	'SL'  : 0b01101 << 11,
	'SR'  : 0b01110 << 11,
	'JUMP': 0b01111 << 11,
	'BREQ': 0b10000 << 11,
	'BRNE': 0b10001 << 11,
	'BRGT': 0b10010 << 11,
	'BRLT': 0b10011 << 11,
	'LDM' : 0b10100 << 11,
	'SVM' : 0b10101 << 11,
	'PUSH': 0b10110 << 11,
	'POP' : 0b10111 << 11,
	'CALL': 0b11000 << 11,
	'RET' : 0b11001 << 11
}

# name: instruction count
pseudo_instructions = {
    'LDI': 2
}

# name: value
registers = {
    'R1': 0b000,
    'R2': 0b001,
    'R3': 0b010,
    'R4': 0b011,
    'R5': 0b100,
    'R6': 0b101,
    'R7': 0b110,
    'R8': 0b111
}

src = read_file(argv[1])

tokens = tokenize(src, instructions, pseudo_instructions, registers)
generator = Generator(tokens, instructions, pseudo_instructions, registers)
write_program_file(argv[2], generator.program)
