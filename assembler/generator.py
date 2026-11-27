class Generator:
    def __init__(self, tokens, instructions, pseudo_instructions, registers):
        self.tokens = tokens
        self.instructions = instructions
        self.pseudo_instructions = pseudo_instructions
        self.registers = registers

        self.iterator = 0
        self.symbols = {}
        self.program = []

        self.generate_symbol_table()
        self.generate_program()

    def peek_token(self):
        return self.tokens[self.iterator]

    def consume_token(self):
        return_token = self.tokens[self.iterator]
        self.iterator += 1
        return return_token

    def consume_n_tokens(self, n):
        self.iterator += n - 1
        return_token = self.tokens[self.iterator]
        self.iterator += 1
        return return_token

    def consume_value(self):
        value_token = self.consume_token()

        match value_token.type:
            case 'ID':
                value = self.symbols[value_token.value]
            case 'NUMBER':
                value = value_token.value
            case _:
                value = 0

        return value

    def consume_register(self):
        register_token = self.consume_token()

        return self.registers[register_token.value]

    def generate_symbol_table(self):
        self.iterator = 0
        current_address = 0

        current_token = self.consume_token()
        while current_token.type != 'END':
            match current_token.type:
                case 'LABEL':
                    self.symbols[current_token.value] = current_address
                    current_token = self.consume_token()
                case 'INSTRUCTION':
                    current_address += 1
                    current_token = self.consume_token()
                case 'PSEUDO_INSTRUCTION':
                    current_address += self.pseudo_instructions[current_token.value]
                    current_token = self.consume_token()
                case 'DIRECTIVE':
                    match current_token.value:
                        case 'DEF':
                            symbol_id = self.consume_token().value
                            symbol_value = self.consume_token().value
                            self.symbols[symbol_id] = symbol_value
                            current_token = self.consume_token()
                        case 'RESERVE':
                            current_address += self.consume_value()
                            current_token = self.consume_token()
                        case _:
                            current_token = self.consume_token()
                case _:
                    current_token = self.consume_token()

    def generate_program(self):
        self.iterator = 0
        current_address = 0

        current_token = self.consume_token()
        while current_token.type != 'END':
            match current_token.type:
                case 'LABEL':
                    current_token = self.consume_token()
                case 'DIRECTIVE':
                    match current_token.value:
                        case 'DEF':
                            current_token = self.consume_n_tokens(3)
                        case 'RESERVE':
                            for i in range(self.consume_token().value):
                                self.program.append(self.instructions['NOP'])
                                current_address += 1

                            current_token = self.consume_token()
                        case _:
                            raise RuntimeError(
                                f'"{current_token.value}({current_token.type})"'
                                + f' invalid directive on line {current_token.line}'
                            )
                case 'INSTRUCTION':
                    match current_token.value:
                        case 'NOP':
                            self.program.append(self.instructions['NOP'])
                        case 'LDHI':
                            self.program.append(
                                self.instructions['LDHI']
                                | self.consume_register() << 8
                                | self.consume_value()
                            )
                        case 'LDLI':
                            self.program.append(
                                self.instructions['LDLI']
                                | self.consume_register() << 8
                                | self.consume_value()
                            )
                        case 'ADD':
                            self.program.append(
                                self.instructions['ADD']
                                | self.consume_register() << 8
                                | self.consume_register() << 3
                                | self.consume_register()
                            )
                        case 'JUMP':
                            self.program.append(
                                self.instructions['JUMP']
                                | self.consume_register() << 8
                            )
                        case 'BRNE':
                            self.program.append(
                                self.instructions['BRNE']
                                | self.consume_register() << 8
                                | self.consume_register() << 3
                                | self.consume_register()
                            )
                        case 'LDM':
                            self.program.append(
                                self.instructions['LDM']
                                | self.consume_register() << 8
                                | self.consume_register()
                            )
                        case 'SVM':
                            self.program.append(
                                self.instructions['SVM']
                                | self.consume_register() << 8
                                | self.consume_register()
                            )
                        case _:
                            raise RuntimeError(f'Unknown instruction "{current_token.value}"')

                    current_address += 1
                    current_token = self.consume_token()
                case 'PSEUDO_INSTRUCTION':
                    match current_token.value:
                        case 'LDI':
                            register = self.consume_register()
                            value = self.consume_value()

                            self.program.append(
                                self.instructions['LDHI']
                                | register << 8
                                | (value & 0xff00) >> 8
                            )

                            self.program.append(
                                self.instructions['LDLI']
                                | register << 8
                                | value & 0x00ff
                            )

                            current_address += 2

                    current_token = self.consume_token()
                case _:
                    raise RuntimeError(
                        f'"{current_token.value}({current_token.type})"'
                        + f' invalid token on line {current_token.line}'
                    )
