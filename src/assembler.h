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

class Assembler
{
private:
	const std::unordered_map<std::string, unsigned char> op_opcodes
	{
		{"ret", 0xc3}
	};

	const std::unordered_map<std::string, unsigned char> mov_register_opcodes
	{
		{"eax", 0xb8},
		{"ebx", 0xbb},
		{"ecx", 0xb9},
		{"edx", 0xba},
		{"esp", 0xbc},
		{"ebp", 0xbd},
		{"esl", 0xbe},
		{"edi", 0xbf}
	};

#define TEXT_ADDR  0x40000078

	int text_offset;

	Elf64_Ehdr header;
	Elf64_Phdr text_program_header;

	std::vector<std::string> functions;

	std::vector<unsigned char> text;

	void _generate_header();
	void _generate_program_header();
	void _generate_text(const std::string &p_input_file);

	char _get_mov_register_opcode(const std::string &p_register);
	int _get_next_columm(const std::string &p_text, int p_column);

	void _push_int(std::vector<unsigned char> &p_vector, int p_value);
	void _push_string(std::vector<unsigned char> &p_vector, std::string p_string);
public:
	void assemble(const std::string &p_input_file, const std::string &p_output_file);

	Assembler();
};

#endif // ASSEMBLER_H
