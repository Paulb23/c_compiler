/*************************************************************************/
/*  assembler.h                                                          */
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

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <elf.h>

#include "tokens.h"

class Assembler
{
private:
	const std::unordered_map<std::string, unsigned char> prefix_opcodes
	{
		{"mul", 0x0F},
	};

	const std::unordered_map<std::string, unsigned char> op_opcodes
	{
		{"add", 0x01},

		{"push_eax", 0x50},
		{"push_ecx", 0x51},
		{"push_edx", 0x52},
		{"push_ebx", 0x53},
		{"push_esp", 0x54},
		{"push_ebp", 0x55},
		{"push_esi", 0x56},
		{"push_esi", 0x57},
		{"push_imm", 0x68},

		{"pop_eax", 0x58},
		{"pop_ecx", 0x59},
		{"pop_edx", 0x5A},
		{"pop_ebx", 0x5B},
		{"pop_esp", 0x5C},
		{"pop_ebp", 0x5D},
		{"pop_esi", 0x5E},
		{"pop_esi", 0x5F},

		{"mul", 0xAF},
		{"ret", 0xc3}
	};

	const std::unordered_map<std::string, unsigned char> register_values
	{
		{"eax", 0x00},
		{"ecx", 0x01},
		{"edx", 0x02},
		{"ebx", 0x03},
		{"esp", 0x04},
		{"ebp", 0x05},
		{"esi", 0x06},
		{"edi", 0x07}
	};

	enum Mod
	{
		REGISTER_INDIRECT_ADRESSING = 0x00,
		ONE_BYTE_DISPLACEMENT = 0x40,
		FOUR_BYTE_DISPLACEMENT = 0x80,
		REGISTER_ADRESSING = 0xC0
	};

	typedef struct Argument
	{
		const Token type;
		const std::string value;
	} Argument;

#define TEXT_ADDR  0x40000078

	Elf64_Ehdr header;
	Elf64_Phdr text_program_header;

	std::vector<std::string> functions;

	std::vector<unsigned char> text;

	void _generate_header();
	void _generate_program_header();
	void _generate_text(const std::string &p_input_file);

	void _push_opcode(
			std::string p_mnemonic,
			Argument p_source = {NONE, ""},
			Argument p_destination = {NONE, ""}
	);

	void _push_int(std::vector<unsigned char> &p_vector, int p_value);
	void _push_string(std::vector<unsigned char> &p_vector, std::string p_string);

	/*
	 * Assembeler Lexer parser
	 */
	struct Node
	{
		Token type;
		Token op;
		std::string value;
	};
	std::string assembly_code;
	int assembly_code_size;
	int assembly_offset;
	int assembly_line;

	void _error(std::string p_error);

	void _load_assembly(const std::string &p_file);

	Node _advance();

	Node _make_node(Token p_token, std::string p_value, Token p_op = NONE);

	Token _get_op_type(const std::string &p_instruction);

	char _get_next_char();
	char _look_ahead(const int p_amount);
public:
	void assemble(const std::string &p_input_file, const std::string &p_output_file);

	Assembler();
};

#endif // ASSEMBLER_H
