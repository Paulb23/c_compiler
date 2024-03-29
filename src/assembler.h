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
#include <set>
#include <elf.h>

#include "tokens.h"

class Assembler
{
private:
	const std::unordered_map<std::string, unsigned char> prefix_opcodes
	{
		{"sub",  0x48},
		{"mul", 0x0F},

		{"mov_dreg", 0x48},
		{"mov_sreg", 0x48},

		{"cmp",  0x48},
	};

	const std::unordered_map<std::string, unsigned char> op_opcodes
	{
		{"add", 0x01},
		{"sub", 0x29},

		{"inc_eax", 0xC0},
		{"inc_ecx", 0xC1},
		{"inc_edx", 0xC2},
		{"inc_ebx", 0xC3},
		{"inc_esp", 0xC4},
		{"inc_ebp", 0xC5},
		{"inc_esi", 0xC6},
		{"inc_edi", 0xC7},

		{"dec_eax", 0xC8},
		{"dec_ecx", 0xC9},
		{"dec_edx", 0xCA},
		{"dec_ebx", 0xCB},
		{"dec_esp", 0xCC},
		{"dec_ebp", 0xCD},
		{"dec_esi", 0xCE},
		{"dec_edi", 0xCF},

		{"push_eax", 0x50},
		{"push_ecx", 0x51},
		{"push_edx", 0x52},
		{"push_ebx", 0x53},
		{"push_esp", 0x54},
		{"push_ebp", 0x55},
		{"push_esi", 0x56},
		{"push_edi", 0x57},
		{"push_imm", 0x68},

		{"pop_eax", 0x58},
		{"pop_ecx", 0x59},
		{"pop_edx", 0x5A},
		{"pop_ebx", 0x5B},
		{"pop_esp", 0x5C},
		{"pop_ebp", 0x5D},
		{"pop_esi", 0x5E},
		{"pop_edi", 0x5F},

		{"mov_dreg",     0x89},
		{"mov_sreg",     0x8B},

		{"mul", 0xAF},
		{"ret", 0xC3},

		{"test", 0x85},

		{"call", 0xE8},
		{"cmp",  0x3B},

		{"jo",    0x70},
		{"jno",   0x71},
		{"jb",    0x72},
		{"jnae",  0x72},
		{"jc",    0x72},
		{"jnb",   0x73},
		{"jae",   0x73},
		{"jnc",   0x73},
		{"je",    0x74},
		{"jz",    0x74},
		{"jne",   0x75},
		{"jnz",   0x75},
		{"jbe",   0x76},
		{"jna",   0x76},
		{"ja",    0x77},
		{"jnbe",  0x77},
		{"js",    0x78},
		{"jns",   0x79},
		{"jp",    0x7A},
		{"jse",   0x7A},
		{"jnp",   0x7B},
		{"jpo",   0x7B},
		{"jl",    0x7C},
		{"jnge",  0x7C},
		{"jge",   0x7D},
		{"jnl",   0x7D},
		{"jle",   0x7E},
		{"jng",   0x7E},
		{"jg",    0x7F},
		{"jnle",  0x7F},
		{"jcxz",  0xE3},
		{"jecxz", 0xE3},
		{"jmp",  0xEB}
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
		const int displacement;
	} Argument;

	/*
	 * For Assembeler Lexer parser
	 */
	struct Node
	{
		Token type;
		Token op;
		std::string value;
	};

#define TEXT_ADDR  0x40000078

	Elf64_Ehdr header;
	Elf64_Phdr text_program_header;

	std::vector<std::string> functions;

	std::vector<unsigned char> text;

	void _generate_header();
	void _generate_program_header();
	void _generate_text(const std::string &p_input_file);

	Argument _calulate_displacement_argument(Node p_node);

	void _push_opcode(
			std::string p_mnemonic,
			Argument p_source = {NONE, "", 0},
			Argument p_destination = {NONE, "", 0}
	);

	void _push_int(std::vector<unsigned char> &p_vector, int p_value);
	void _push_string(std::vector<unsigned char> &p_vector, std::string p_string);

	/*
	 * Assembeler Lexer parser
	 */
	const std::set<std::string> jump_operations
	{
		"jo",
		"jno",
		"jb",
		"jnae",
		"jc",
		"jnb",
		"jae",
		"jnc",
		"je",
		"jz",
		"jne",
		"jnz",
		"jbe",
		"jna",
		"ja",
		"jnbe",
		"js",
		"jns",
		"jp",
		"jse",
		"jnp",
		"jpo",
		"jl",
		"jnge",
		"jge",
		"jnl",
		"jle",
		"jng",
		"jg",
		"jnle",
		"jcxz",
		"jecxz",
		"jmp"
	};

	std::string assembly_code;
	int assembly_code_size;
	int assembly_offset;
	int assembly_line;

	void _error(std::string p_error);

	void _load_assembly(const std::string &p_file);

	Node _peek();
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
