/*************************************************************************/
/*  compiler.cpp                                                         */
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

#include "compiler.h"


void Compiler::compile(const std::string &p_file_path)
{
	std::unique_ptr<TreeNode<Parser::Node>> ast = parser.parse(p_file_path);

	const std::string assembly_file_name = p_file_path.substr(0, p_file_path.find_last_of('.')) + ".s";
	code_generator.generate_code(ast, assembly_file_name);

	const std::string elf_file_name = p_file_path.substr(0, p_file_path.find_last_of('.'));
	assembler.assemble(assembly_file_name, elf_file_name);
}

Compiler::Compiler()
{

}
