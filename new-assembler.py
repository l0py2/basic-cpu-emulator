#!/usr/bin/env python3

# https://docs.python.org/3/library/re.html#writing-a-tokenizer

from typing import NamedTuple
import re

class Token(NamedTuple):
    type: str
    value: str
    line: int
    column: int

def tokenize(code):
    keywords = {
        'NOP',
        'LDHI', 'LDLI', 'LDM', 'SVM', 'COPY',
        'ADDI', 'SUBI', 'ADD', 'SUB', 'MULT', 'DIV', 'MOD',
        'COMPI', 'COMP',
        'SLI', 'SRI', 'CLR', 'SET', 'NOT', 'AND', 'OR',
        'JUMP', 'BRNEQ', 'BREQ', 'CALL',
        'PUSH', 'POP', 'RET'
    }

    token_specification = [
        ('COMMENT', r'# [^\n]*'),
        ('HEX', r'0x[0-9a-fA-F]+'),
        ('BIN', r'0b[01]+'),
        ('DEC', r'[0-9]'),
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

with open('./examples/basic.scasm', 'r') as file:
    src = file.read()

    for token in tokenize(src):
        print(token)
