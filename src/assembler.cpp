/*************************************************************************/
/*  assembler.cpp                                                        */
/*************************************************************************/
/*                       The MIT License (MIT)                           */
/*************************************************************************/
/* Copyright (c) 2018 Paul Batty.                                        */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "assembler.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <elf.h>

static bool _is_number(const char &c)
{
	return (c >= '0' && c <= '9');
}

static bool _is_text_char(const char &c)
{
	return  (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			_is_number(c)          ||
			c == '_';
}

void Assembler::assemble(
		const std::string &p_input_file,
		const std::string &p_output_file
) {
	_generate_header();
	_generate_program_header();

	_generate_text(p_input_file);

	text_program_header.p_filesz = text.size() * sizeof(unsigned char);
	text_program_header.p_memsz = text.size() * sizeof(unsigned char);

	std::ofstream file;
	file.open(p_output_file);

	file.write((char *)&header, sizeof(Elf64_Ehdr));
	file.write((char *)&text_program_header, sizeof(Elf64_Phdr));

	for(unsigned int i=0; i< text.size(); ++i)
	{
		std::cout << std::hex << (int)text[i];
	}

	for (const char opcode : text)
	{
		file.write((const char *)&opcode, 1);
	}

	file.close();
}

void Assembler::_generate_header()
{
	header.e_ident[EI_MAG0] = ELFMAG0;
	header.e_ident[EI_MAG1] = ELFMAG1;
	header.e_ident[EI_MAG2] = ELFMAG2;
	header.e_ident[EI_MAG3] = ELFMAG3;
	header.e_ident[EI_CLASS] = ELFCLASS64;
	header.e_ident[EI_DATA] = ELFDATA2LSB;
	header.e_ident[EI_VERSION] = EV_CURRENT;
	header.e_ident[EI_OSABI] = ELFOSABI_NONE;
	header.e_ident[EI_ABIVERSION] = 0;
	header.e_ident[EI_PAD] = 0;

	header.e_type = ET_EXEC;
	header.e_machine = EM_X86_64;
	header.e_version = EV_CURRENT;
	header.e_entry = TEXT_ADDR;
	header.e_phoff = sizeof(Elf64_Ehdr);
	header.e_shoff = 0;
	header.e_flags = 0;
	header.e_ehsize = sizeof(Elf64_Ehdr);
	header.e_phentsize = sizeof(Elf64_Phdr);
	header.e_phnum = 1;
	header.e_shentsize = sizeof(Elf64_Shdr);
	header.e_shnum = 0;
	header.e_shstrndx = 0;
}

void Assembler::_generate_program_header()
{
	text_program_header.p_type = PT_LOAD;
	text_program_header.p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
	text_program_header.p_vaddr = TEXT_ADDR;
	text_program_header.p_paddr = 0;
	text_program_header.p_filesz = 0;
	text_program_header.p_memsz = 0;
	text_program_header.p_flags = PF_R | PF_X;
	text_program_header.p_align = 0;
}

void Assembler::_generate_text(const std::string &p_input_file)
{
	_load_assembly(p_input_file);

	std::unordered_map<std::string, unsigned char> label_addresses;
	std::unordered_map<int, std::string> pending_addresses;
	std::unordered_map<int, int> instruction_size; /* TODO: need to keep better track */

	Node node = _advance();
	while (node.type != TK_EOF && node.type != TK_ERROR)
	{
		switch (node.type)
		{
			case TK_GLOB:
			{
				// TODO: implement
			} break;
			case TK_IDENTIFIER:
			{
				if (!label_addresses.count(node.value))
				{
					unsigned int address = TEXT_ADDR + (text.size() * sizeof(unsigned char));
					label_addresses[node.value] = address;

					for (std::pair<int, std::string> jump : pending_addresses)
					{
						if (jump.second != node.value)
						{
							continue;
						}

						unsigned char relative_address = (text.size() - jump.first) - instruction_size[jump.first];
						text[jump.first + 1] = ((relative_address) & 0xFF);
					}
				}

				node = _advance();

				if (node.type != TK_COLON)
				{
					_error("expected ':' but found '" + node.value + "'");
				}
			} break;
			case TK_TEST:
			{
				node = _advance();
				Argument source{node.type, node.value, ""};

				/* skip comma */
				node = _advance();

				node = _advance();
				Argument destination{node.type, node.value, ""};

				_push_opcode("test", source, destination);
			} break;
			case TK_CALL:
			{
				/* TODO: merge with JMP? */
				std::string jump_type = node.value;
				node = _advance();

				if (label_addresses.count(node.value))
				{
					// TODO: label already exists.
				}
				else
				{
					pending_addresses[text.size()] = node.value;
					instruction_size[text.size()] = 5;
					Argument value{TK_CONSTANT, "0", ""};
					_push_opcode(jump_type);
					text.push_back(0x0);
					text.push_back(0x0);
					text.push_back(0x0);
					text.push_back(0x0);
				}
			} break;
			case TK_JMP:
			{
				std::string jump_type = node.value;
				node = _advance();

				if (label_addresses.count(node.value))
				{
					// TODO: label already exists.
				}
				else
				{
					pending_addresses[text.size()] = node.value;
					instruction_size[text.size()] = 2;
					Argument value{TK_CONSTANT, "0", ""};
					_push_opcode(jump_type);
					text.push_back(0x0);
				}
			} break;
			case TK_PUSH:
			{
				node = _advance();
				switch (node.type)
				{
					case TK_CONSTANT:
					{
						Argument value{node.type, node.value, ""};
						_push_opcode("push_imm", value);
					} break;
					case TK_REGISTER:
					{
						_push_opcode("push_" + node.value);
					} break;
					default:
					{
						_error("expected constant or register but found '" + node.value + "'");
					} break;
				}
			} break;
			case TK_POP:
			{
				node = _advance();
				if (node.type != TK_REGISTER)
				{
					_error("expected register but found '" + node.value + "'");
				}
				_push_opcode("pop_" + node.value);
			} break;
			case TK_ADD:
			{
				node = _advance();
				Argument source{node.type, node.value, ""};

				/* skip comma */
				node = _advance();

				node = _advance();
				Argument destination{node.type, node.value, ""};

				_push_opcode("add", source, destination);
			} break;
			case TK_MUL:
			{
				node = _advance();
				Argument source{node.type, node.value, ""};

				/* skip comma */
				node = _advance();

				node = _advance();
				Argument destination{node.type, node.value, ""};

				_push_opcode("mul", source, destination);
			} break;
			case TK_MOV:
			{
				node = _advance();

				Argument source = _calulate_displacement_argument(node);

				/* skip comma */
				node = _advance();
				node = _advance();

				Argument destination = _calulate_displacement_argument(node);

				if (source.displacement == "")
				{
					_push_opcode("mov_dreg", source, destination);
				}
				else
				{
					_push_opcode("mov_sreg", source, destination);
				}
			} break;
			case TK_RET:
			{
				node = _advance();

				_push_opcode("ret");
			} break;
			case TK_SYSCALL:
			{
				node = _advance();

				text.push_back(0x0F);
				text.push_back(0x05);
			} break;
		}

		//std::cout << token_to_string.at(node.type) << " " << token_to_string.at(node.op) << " " << node.value << std::endl;
		node = _advance();
	}
}

Assembler::Argument Assembler::_calulate_displacement_argument(Node p_node)
{
	Token type = p_node.type;
	std::string value = p_node.value;
	std::string displacement = "";
	if (p_node.type == TK_MINUS)
	{
		p_node = _advance(); // -
		displacement = p_node.value;
		p_node = _advance(); // constant
		type = p_node.type,
		value = p_node.value;
	}
	return Argument{type, value, displacement};
}

void Assembler::_push_opcode(
		std::string p_mnemonic,
		Argument p_source,
		Argument p_destination
) {
	/*
	 *                           operand
	 *  prefix  | opcode d s | MOD REG R/M | immediate
	 * 00000000 | 000000 0 0 | 00  000 000 | 00000000
	 */
	bool has_prefix = false;
	bool has_operand = false;
	bool has_immediate = false;

	unsigned char prefix;
	if (prefix_opcodes.count(p_mnemonic))
	{
		has_prefix = true;
		prefix = prefix_opcodes.at(p_mnemonic);
	}

	unsigned char opcode = op_opcodes.at(p_mnemonic);

	unsigned char operand;
	std::vector<unsigned char> immediate;
	if (p_source.type != NONE || p_destination.type != NONE)
	{

		if (p_source.type == TK_CONSTANT)
		{
			has_immediate = true;
			_push_int(immediate, std::stoi(p_source.value));
		}
		else
		{
			has_operand = true;
			operand = 0x00;
			bool source_is_dest = (opcode & 2) != 0;
			Argument reg = source_is_dest ? p_destination : p_source;
			Argument rm =  source_is_dest ? p_source : p_destination;

			unsigned char mod = 0x00;
			if (rm.type == TK_REGISTER && rm.displacement == "")
			{
				mod = REGISTER_ADRESSING;
			}
			else
			{
				mod = FOUR_BYTE_DISPLACEMENT;
			}
			operand |= mod;

			operand |= register_values.at(reg.value) << 3;
			operand |= register_values.at(rm.value);
			if (mod == FOUR_BYTE_DISPLACEMENT)
			{
				if (!has_immediate && rm.displacement != "")
				{
					has_immediate = true;
					int displacement = std::stoi(rm.displacement);
					if (std::stoi(rm.displacement) == 0)
					{
						displacement = 0x00000000;
					}
					else
					{
						displacement = 0xFFFFFFFF ^(displacement - 1);
					}
					_push_int(immediate, displacement);
				}
			}
		}
	}

	if (has_prefix)
	{
		text.push_back(prefix);
	}

	text.push_back(opcode);

	if (has_operand)
	{
		text.push_back(operand);
	}

	if (has_immediate)
	{
		for (unsigned char c : immediate)
		{
			text.push_back(c);
		}
	}

}

void Assembler::_push_int(std::vector<unsigned char> &p_vector, int p_value)
{
	p_vector.push_back(p_value & 0xFF);
	p_vector.push_back((p_value >> 8) & 0xFF);
	p_vector.push_back((p_value >> 16) & 0xFF);
	p_vector.push_back((p_value >> 24) & 0xFF);
}

void Assembler::_push_string(std::vector<unsigned char> &p_vector, std::string p_string)
{
	for (char c : p_string)
	{
		p_vector.push_back(c);
	}

	// add null terminator
	p_vector.push_back(0);
}

/*
 * Assembeler Lexer parser
 */

void Assembler::_error(std::string p_error)
{
	std::cout << "assembler: line " << (assembly_line);
	std::cout << ": error: " << p_error << std::endl;
	exit(0);
}

void Assembler::_load_assembly(const std::string &p_file)
{
	const int buffer_size = 4096;
	std::unique_ptr<char[]> buffer(new char[buffer_size]);
	std::ifstream stream(p_file);
	while (stream)
	{
		stream.read(buffer.get(), buffer_size);
		assembly_code.append(buffer.get());
	}
	stream.close();
	assembly_code_size = assembly_code.length();
	assembly_offset = -1;
	assembly_line = 0;
}

Assembler::Node Assembler::_advance()
{
	while (true)
	{
		const char &c = _get_next_char();
		switch(c)
		{
			case 0:
			{
				return _make_node(TK_EOF, "eof");
			} break;

			case '\r':
				if (_look_ahead(1) != '\n')
				{
					return _make_node(TK_ERROR, "expected new line.");
				}
				_get_next_char();
				// fallthrough to new line
			case '\n':
				assembly_line++;
				return _make_node(TK_NEWLINE, "\n");

			// whitespace
			case ' ':
			case '\t':
				continue;

			case '#':
			{
				while (true)
				{
					const char &cc = _get_next_char();
					if (cc == '\n' || (cc == '\r' && _look_ahead(1) == '\n'))
					{
						break;
					}
				}
				assembly_offset--;
				continue;
			} break;
			case '/':
			{
				if (_look_ahead(1) != '*')
				{
					_error("expected multiline comment");
				}

				_get_next_char();
				while (true)
				{
					const char &cc = _get_next_char();
					if (cc == '\n' || (cc == '\r' && _look_ahead(1) == '\n'))
					{
						assembly_line++;
					}

					if (cc == '0' || (cc == '*' && _look_ahead(1) == '/'))
					{
						_get_next_char();
						break;
					}
				}
				continue;
			} break;
			case ':':
			{
				return _make_node(TK_COLON, ":");
			} break;
			case '-':
			{
				return _make_node(TK_MINUS, "-");
			} break;
			case ',':
			{
				return _make_node(TK_COMMA, ",");
			} break;
			case '$':
			{
				std::string value;
				Node tk_value = _advance();
				if (tk_value.type == TK_MINUS)
				{
					value != "-";
					tk_value = _advance();
				}

				if (tk_value.type != TK_CONSTANT)
				{
					_error("expetect number but found '" + tk_value.value + "'" );
				}
				value += tk_value.value;
				return _make_node(TK_CONSTANT, value);
			} break;
			/* skip parens for now*/
			case '(':
			case ')':
			{
				continue;
			} break;
			case '%':
			{
				Node tk_value = _advance();
				if (tk_value.type != TK_IDENTIFIER)
				{
					_error("expetect identifier but found '" + tk_value.value + "'" );
				}
				return _make_node(TK_REGISTER, tk_value.value);
			} break;
			default:
			{
				if (_is_number(c))
				{
					int i = 0;
					std::string number;
					while (_is_number(_look_ahead(i)))
					{
						number += _look_ahead(i);
						i++;
					}
					assembly_offset += i - 1;
					return _make_node(TK_CONSTANT, number);
				}

				int i = 0;
				std::string word;
				while (_is_text_char(_look_ahead(i)))
				{
					word += _look_ahead(i);
					i++;
				}
				assembly_offset += i - 1;

				if (word == "globl")
				{
					Node value = _advance();
					if (value.type != TK_IDENTIFIER)
					{
						_error("expected identifier, but found: '" + token_to_string.at(value.type) + "'" );
					}
					return _make_node(TK_GLOB, value.value);
				}

				if (word.find("push") == 0)
				{
					return _make_node(TK_PUSH, word, _get_op_type(word));
				}

				if (word.find("pop") == 0)
				{
					return _make_node(TK_POP, word, _get_op_type(word));
				}

				if (word.find("add") == 0)
				{
					return _make_node(TK_ADD, word, _get_op_type(word));
				}

				if (word.find("mul") == 0)
				{
					return _make_node(TK_MUL, word, _get_op_type(word));
				}

				if (word.find("mov") == 0)
				{
					return _make_node(TK_MOV, word, _get_op_type(word));
				}

				if (word.find("ret") == 0)
				{
					return _make_node(TK_RET, word);
				}

				if (word.find("jz") == 0 || word.find("jmp") == 0)
				{
					return _make_node(TK_JMP, word);
				}

				if (word.find("syscall") == 0)
				{
					return _make_node(TK_SYSCALL, word);
				}

				if (word.find("call") == 0)
				{
					return _make_node(TK_CALL, word);
				}

				if (word.find("test") == 0)
				{
					return _make_node(TK_TEST, word);
				}

				return _make_node(TK_IDENTIFIER, word);
			} break;
		}
		return _make_node(TK_ERROR, "unkown token");
	}
}

Token Assembler::_get_op_type(const std::string &p_instruction)
{
	if (p_instruction.length() <= 3)
	{
		return OP_NONE;
	}

	std::string type = p_instruction.substr(3, p_instruction.length());
	if (type == "b")
	{
		return OP_BYTE;
	}

	if (type == "s")
	{
		return OP_SHORT;
	}

	if (type == "w")
	{
		return OP_WORD;
	}

	if (type == "l")
	{
		return OP_LONG;
	}

	if (type == "q")
	{
		return OP_QUAD;
	}

	if (type == "t")
	{
		return OP_TEN_BYTE;
	}

	return OP_NONE;
}

Assembler::Node Assembler::_make_node(Token p_token, std::string p_value, Token p_op)
{
	Node node;
	node.type = p_token;
	node.value = p_value;
	node.op = p_op;
	return node;
}

char Assembler::_get_next_char()
{
	assembly_offset++;
	return _look_ahead(0);
}

char Assembler::_look_ahead(const int p_amount)
{
	if (assembly_offset + p_amount >= assembly_code_size)
	{
		return 0;
	}
	return assembly_code[assembly_offset + p_amount];
}

Assembler::Assembler()
{
}
