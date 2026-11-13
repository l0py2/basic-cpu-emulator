def generate_symbol_table(tokens, pseudo_instructions):
    symbol_table = {}
    current_address = 0

    current_token = next(tokens)

    while current_token.type != 'END':
        match current_token.type:
            case 'LABEL':
                symbol_table[current_token.value] = current_address
                current_token = next(tokens)
            case 'INSTRUCTION':
                current_address += 1
                current_token = next(tokens)
            case 'PSEUDO_INSTRUCTION':
                current_address += pseudo_instructions[current_token.value]
                current_token = next(tokens)
            case 'DIRECTIVE':
                match current_token.value:
                    case 'DEF':
                        symbol_id = next(tokens).value
                        symbol_value = next(tokens).value
                        symbol_table[symbol_id] = symbol_value
                        current_token = next(tokens)
                    case 'RESERVE':
                        current_address += next(tokens).value
                        current_token = next(tokens)
                    case _:
                        current_token = next(tokens)
            case _:
                current_token = next(tokens)

    return symbol_table

def generate_program(tokens, symbol_table, instructions, pseudo_instructions, registers):
    program = []
    current_address = 0

    current_token = next(tokens)

    while current_token.type != 'END':
        match current_token.type:
            case 'LABEL':
                current_token = next(tokens)
            case 'DIRECTIVE':
                match current_token.value:
                    case 'DEF':
                        next(tokens)
                        next(tokens)
                        current_token = next(tokens)
                    case 'RESERVE':
                        for i in range(next(tokens).value):
                            program.append(instructions['NOP'])
                            current_address += 1

                        current_token = next(tokens)
                    case _:
                        raise RuntimeError(
                            f'"{current_token.value}({current_token.type})"'
                            + f' invalid directive on line {current_token.line}'
                        )
            case 'INSTRUCTION' | 'PSEUDO_INSTRUCTION':
                match current_token.value:
                    case 'NOP':
                        program.append(instructions['NOP'])
                        current_address += 1
                        current_token = next(tokens)
                    case 'LDHI':
                        program.append(
                            instructions['LDHI']
                            | registers[next(tokens).value] << 8
                            | next(tokens).value
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'LDLI':
                        program.append(
                            instructions['LDLI']
                            | registers[next(tokens).value] << 8
                            | next(tokens).value
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'ADDI':
                        program.append(
                            instructions['ADDI']
                            | registers[next(tokens).value] << 8
                            | (next(tokens).value & 0x00ff)
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'RJUMP':
                        program.append(
                            instructions['RJUMP']
                            | ((symbol_table[next(tokens).value] - current_address) & 0x00ff)
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'LDM':
                        program.append(
                            instructions['LDM']
                            | registers[next(tokens).value] << 8
                            | registers[next(tokens).value]
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'SVM':
                        program.append(
                            instructions['SVM']
                            | registers[next(tokens).value] << 8
                            | registers[next(tokens).value]
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'PUSH':
                        program.append(
                            instructions['SVM']
                            | registers[next(tokens).value] << 8
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'POP':
                        program.append(
                            instructions['SVM']
                            | registers[next(tokens).value] << 8
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'RCALL':
                        program.append(
                            instructions['RCALL']
                            | ((symbol_table[next(tokens).value] - current_address) & 0x00ff)
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'RET':
                        program.append(instructions['RET'])
                        current_address += 1
                        current_token = next(tokens)
                    case 'RBREZ':
                        program.append(
                            instructions['RBREZ']
                            | registers[next(tokens).value] << 8
                            | ((symbol_table[next(tokens).value] - current_address) & 0x00ff)
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'RBRNZ':
                        program.append(
                            instructions['RBRNZ']
                            | registers[next(tokens).value] << 8
                            | ((symbol_table[next(tokens).value] - current_address) & 0x00ff)
                        )
                        current_address += 1
                        current_token = next(tokens)
                    case 'LDI':
                        register = registers[next(tokens).value]
                        value = 0

                        value_token = next(tokens)
                        match value_token.type:
                            case 'ID':
                                value = symbol_table[value_token.value]
                            case 'NUMBER':
                                value = value_token.value
                            case _:
                                raise RuntimeError(
                                    f'"{value_token.value}({value_token.type})"'
                                    + f' invalid token on line {value_token.line}'
                                )

                        program.append(
                            instructions['LDHI']
                            | register << 8
                            | (value & 0xff00) >> 8
                        )

                        program.append(
                            instructions['LDLI']
                            | register << 8
                            | value & 0x00ff
                        )

                        current_address += 2
                        current_token = next(tokens)
            case _:
                raise RuntimeError(
                    f'"{current_token.value}({current_token.type})"'
                    + f' invalid token on line {current_token.line}'
                )

    return program
