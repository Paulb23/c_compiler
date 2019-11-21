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
#include <unordered_map>

#include "tokens.h"
#include "symantic_analysier.h"

class CodeGenerator
{
private:

	std::vector<SymanticAnalysier::Node> _create_list(
			const std::unique_ptr<TreeNode<SymanticAnalysier::Node>> &p_root,
			unsigned int parent_id = 0
	);

	void _error(std::string p_error);
	void _warn(std::string p_warning);

	struct Var {
		unsigned int scope_level = 0;
		int stack_offset = 0;
	};

	struct Scope {
		unsigned int level = 0;
		int stack_offset = 8;
		std::unordered_map<std::string, Var> var_map;
	};

	std::unordered_map<std::string, unsigned int> function_map;

	unsigned int if_counter;
	unsigned int if_clause_counter;

	unsigned int comp_clause_counter;

	unsigned int last_line;
	std::vector<std::string> code;

	unsigned int current_node_offset;
	std::vector<SymanticAnalysier::Node> tree_vector;
	SymanticAnalysier::Node current_node;

	void _advance();
	Token _peek();
	void _append_line(std::string p_code);
	void _set_line(unsigned int p_line, std::string p_code);

	void _generate_program();
	void _generate_function();

	void _generate_code_block(const Scope &p_scope);
	Scope _generate_declaration(const Scope &p_scope);
	void _generate_assignment_expression(const Scope &p_scope);
	void _generate_statement(const Scope &p_scope);
	void _generate_if_block(const Scope &p_scope);
	void _generate_expression(const Scope &p_scope);

public:
	void generate_code(
			std::unique_ptr<TreeNode<SymanticAnalysier::Node>> &p_root,
			const std::string &p_output_file
	);

	CodeGenerator();
};

#endif // CODE_GENERATOR_H
