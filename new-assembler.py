#!/usr/bin/env python3

# https://docs.python.org/3/library/re.html#writing-a-tokenizer

from typing import NamedTuple
import re
from enum import Enum
from itertools import tee

class Instruction(Enum):
    NOP   = 0 << 11
    # Memory
    LDHI  = 1 << 11
    LDLI  = 2 << 11
    LDM   = 3 << 11
    SVM   = 4 << 11
    COPY  = 5 << 11
    # Arithmetic
    ADDI  = 6 << 11
    SUBI  = 7 << 11
    ADD   = 8 << 11
    SUB   = 9 << 11
    MULT  = 10 << 11
    DIV   = 11 << 11
    MOD   = 12 << 11
    # Comparison
    COMPI = 13 << 11
    COMP  = 14 << 11
    # Bit-wise
    SLI   = 15 << 11
    SRI   = 16 << 11
    CLR   = 17 << 11
    SET   = 18 << 11
    NOT   = 19 << 11
    AND   = 20 << 11
    OR    = 10 << 11
    # Branching
    JUMP  = 21 << 11
    BRNEQ = 22 << 11
    BREQ  = 23 << 11
    CALL  = 24 << 11
    # Stack
    PUSH  = 25 << 11
    POP   = 26 << 11
    RET   = 27 << 11

class Register(Enum):
    R1 = 0
    R2 = 1
    R3 = 2
    R4 = 3
    R5 = 4
    R6 = 5
    R7 = 6
    R8 = 7

class Token(NamedTuple):
    type: str
    value: str
    line: int
    column: int

def tokenize(code):
    keywords = []

    for instruction in Instruction:
        keywords.append(instruction.name)

    token_specification = [
        ('COMMENT', r'#[^\n]*'),
        ('HEX_NUM', r'0x[0-9a-fA-F]+'),
        ('BIN_NUM', r'0b[01]+'),
        ('DEC_NUM', r'[0-9]+'),
        ('LABEL', r'[a-zA-Z0-9_]+:'),
        ('REGISTER', r'R[0-9]'),
        ('ID', r'[a-zA-Z0-9_]+'),
        ('NEWLINE', r'\n'),
        ('EMPTY', r'[ \t]+'),
        ('ANY', r'.')
    ]

    token_regex = '|'.join('(?P<%s>%s)' % pair for pair in token_specification)

    line_num = 1
    line_start = 0

    for lexeme in re.finditer(token_regex, code):
        kind = lexeme.lastgroup
        value = lexeme.group()
        column = lexeme.start() - line_start

        if kind == 'ID' and value in keywords:
            kind = value
            value = ''
        elif kind == 'LABEL':
            value = value[:-1]
        elif kind == 'HEX_NUM':
            value = int(value, 16)
        elif kind == 'DEC_NUM':
            value = int(value, 10)
        elif kind == 'BIN_NUM':
            value = int(value, 2)
        elif kind == 'COMMENT':
            continue
        elif kind == 'NEWLINE':
            line_start = lexeme.end()
            line_num += 1
            continue
        elif kind == 'EMPTY':
            continue
        elif kind == 'ANY':
            raise RuntimeError(f'"{value}" invalid on line {line_num}')

        yield Token(kind, value, line_num, column)

    yield Token('END', '', line_num, column)

def tokenize_file(path):
    with open(path, 'r') as file:
        src = file.read()

        return tokenize(src)

def generate_symbol_table(tokens):
    symbol_table = {}
    current_address = 0

    current_token = next(tokens)

    while current_token.type != 'END':
        match current_token.type:
            case 'LABEL':
                symbol_table[current_token.value] = current_address
                current_token = next(tokens)
            case 'NOP':
                current_address += 1
                current_token = next(tokens)
            case 'LDHI':
                next(tokens)
                next(tokens)
                current_address += 1
                current_token = next(tokens)
            case 'LDLI':
                next(tokens)
                next(tokens)
                current_address += 1
                current_token = next(tokens)
            case _:
                # raise RuntimeError(f'"{current_token.type}" invalid token')
                current_token = next(tokens)

    return symbol_table

def generate_program(tokens, symbol_table):
    program = []
    current_address = 0

    current_token = next(tokens)

    while current_token.type != 'END':
        match current_token.type:
            case 'LABEL':
                current_token = next(tokens)
            case 'NOP':
                program.append(Instruction.NOP.value)
                current_address += 1
                current_token = next(tokens)
            case 'LDHI':
                program.append(
                    Instruction.LDHI.value
                    | Register[next(tokens).value].value << 8
                    | next(tokens).value
                )
                current_address += 1
                current_token = next(tokens)
            case 'LDLI':
                program.append(
                    Instruction.LDLI.value
                    | Register[next(tokens).value].value << 8
                    | next(tokens).value
                )
                current_address += 1
                current_token = next(tokens)
            case _:
                # raise RuntimeError(f'"{current_token.type}" invalid token')
                current_token = next(tokens)

    return program

symbol_tokens, program_tokens = tee(tokenize_file('./examples/new-basic.asm'), 2)

symbol_table = generate_symbol_table(symbol_tokens)
program = generate_program(program_tokens, symbol_table)

print('=== Symbol Table ===')
for symbol, value in symbol_table.items():
    print(f'{symbol} = {value}')

print('=== Program Memory ===')
for value in program:
    print(f'0x{format(value, '04x')} = 0b{format(value, '016b')}')
