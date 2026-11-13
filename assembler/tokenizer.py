from typing import NamedTuple
import re

class Token(NamedTuple):
    type: str
    value: str
    line: int
    column: int

def tokenize(code, instructions, pseudo_instructions, registers):
    token_specification = [
        ('COMMENT', r'#[^\n]*'),
        ('HEX_NUM', r'0x[0-9a-fA-F]+'),
        ('BIN_NUM', r'0b[01]+'),
        ('NEG_DEC_NUM', r'-[0-9]+'),
        ('DEC_NUM', r'[0-9]+'),
        ('LABEL', r'[a-zA-Z0-9_]+:'),
        ('DIRECTIVE', r'\.[a-zA-Z0-9_]+'),
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

        if kind == 'ID' and value in instructions:
            kind = 'INSTRUCTION'
        elif kind == 'ID' and value in pseudo_instructions:
            kind = 'PSEUDO_INSTRUCTION'
        elif kind == 'ID' and value in registers:
            kind = 'REGISTER'
        elif kind == 'LABEL':
            value = value[:-1]
        elif kind == 'DIRECTIVE':
            value = value[1:]
        elif kind == 'HEX_NUM':
            kind = 'NUMBER'
            value = int(value, 16)
        elif kind == 'NEG_DEC_NUM':
            kind = 'NUMBER'
            value = int(value, 10)
        elif kind == 'DEC_NUM':
            kind = 'NUMBER'
            value = int(value, 10)
        elif kind == 'BIN_NUM':
            kind = 'NUMBER'
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
