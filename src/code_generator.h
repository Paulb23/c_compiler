/*************************************************************************/
/*  code_generator.h                                                     */
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

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <string>
#include <vector>
#include <memory>

#include "tokens.h"
#include "parser.h"

class CodeGenerator
{
private:
	enum Bits
	{
		EIGHT_BITS,
		SIXTEEN_BITS,
		THRITY_TWO_BITS,
		SIXTY_FOUR_BITS
	};

	const std::string eight_bit_register[16]
	{
		"%al",
		"%bl",
		"%cl",
		"%dl",
		"%sil",
		"%dil",
		"%bpl",
		"%spl",
		"%r8b",
		"%r9b",
		"%r10b",
		"%r11b",
		"%r12b",
		"%r13b",
		"%r14b",
		"%r15b",
	};

	const std::string sixteen_bit_register[16]
	{
		"%ax",
		"%bx",
		"%cx",
		"%dx",
		"%si",
		"%di",
		"%bp",
		"%sp",
		"%r8w",
		"%r9w",
		"%r10w",
		"%r11w",
		"%r12w",
		"%r13w",
		"%r14w",
		"%r15w",
	};

	const std::string thrity_two_bit_register[16]
	{
		"%edi",
		"%eax",
		"%ebx",
		"%ecx",
		"%edx",
		"%esi",
		"%ebp",
		"%esp",
		"%r8d",
		"%r9d",
		"%r10d",
		"%r11d",
		"%r12d",
		"%r13d",
		"%r14d",
		"%r15d",
	};


	const std::string sixty_four_bit_register[16]
	{
		"%rax",
		"%rbx",
		"%rcx",
		"%rdx",
		"%rsi",
		"%rdi",
		"%rbp",
		"%rsp",
		"%r8",
		"%r9",
		"%r10",
		"%r11",
		"%r12",
		"%r13",
		"%r14",
		"%r15",
	};

	std::vector<Parser::Node> _create_list(const std::unique_ptr<TreeNode<Parser::Node>> &p_current_node);

	std::string _get_register(Bits p_bits);
public:
	void generate_code(std::unique_ptr<TreeNode<Parser::Node>> &p_root, const std::string &p_output_file);

	CodeGenerator();
};

#endif // CODE_GENERATOR_H
