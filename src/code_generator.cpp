/*************************************************************************/
/*  code_generator.cpp                                                   */
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


#include "code_generator.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "./data_structures/tree_node.h"

void CodeGenerator::generate_code(
		std::unique_ptr<TreeNode<SymanticAnalysier::Node>> &p_root,
		const std::string &p_output_file
) {
	if_counter = 0;
	if_clause_counter = 0;

	code.clear();
	current_node_offset = -1;
	tree_vector = _create_list(p_root);
	_advance();

	if (current_node.type != TYPE_PROGRAM)
	{
		_error("expected program, but found: '" + token_to_string.at(current_node.type) + "'");
	}

	_advance();
	_generate_program();

	std::cout << "-----------------------------------------------" << std::endl;
	std::ofstream file;
	file.open(p_output_file);
	for (const std::string line : code)
	{
		std::cout << line << std::endl;
		file << line + "\n";
	}
	file << '\0' << std::endl;
	file.close();
	std::cout << "-----------------------------------------------" << std::endl;
}

std::vector<SymanticAnalysier::Node> CodeGenerator::_create_list(
		const std::unique_ptr<TreeNode<SymanticAnalysier::Node>> &p_root,
		unsigned int parent_id
) {
	std::vector<SymanticAnalysier::Node> node_list;
	SymanticAnalysier::Node node = p_root->get_data();
	node.parent_id = parent_id;
	node_list.push_back(node);

	const std::list<std::unique_ptr<TreeNode<SymanticAnalysier::Node>>> &children = p_root->get_children();
	for (const std::unique_ptr<TreeNode<SymanticAnalysier::Node>> &child : children)
	{
		std::vector<SymanticAnalysier::Node> child_nodes = _create_list(child, node.id);
		for (SymanticAnalysier::Node &childs : child_nodes)
		{
			node_list.push_back(childs);
		}
	}
	return node_list;
}

void CodeGenerator::_error(std::string p_error)
{
	std::cout << "error: " << p_error << std::endl;
	exit(0);
}

void CodeGenerator::_advance()
{
	if (current_node_offset + 1 >= tree_vector.size())
	{
		return;
	}
	current_node_offset++;
	current_node = tree_vector[current_node_offset];
}

Token CodeGenerator::_peek()
{
	if (current_node_offset + 1 >= tree_vector.size())
	{
		return TK_SEMICOLON;
	}
	return tree_vector[current_node_offset+1].type;
}

void CodeGenerator::_append_line(std::string p_code)
{
	_set_line(last_line + 1, p_code);
}

void CodeGenerator::_set_line(unsigned int p_line, std::string p_code)
{
	if (p_line > last_line)
	{
		code.push_back(p_code);
		last_line++;
		return;
	}
	code[p_line] = p_code;
}

/*
 * Assembly generation starts here.
 */

void CodeGenerator::_generate_program()
{
	/* we know there's only one for now */
	_generate_function();
}

void CodeGenerator::_generate_function()
{
	_append_line("globl " + current_node.value);
	_append_line(current_node.value + ":");

	_advance();

	// args here
	// advance

	if (current_node.type == CODE_BLOCK)
	{
		_advance();
		_generate_code_block();
	}
}

void CodeGenerator::_generate_code_block()
{
	while (current_node_offset + 1 < tree_vector.size())
	{
		_generate_statement();
	}
}

void CodeGenerator::_generate_statement()
{
	if (current_node.type == TK_IF)
	{
		_generate_if_block();
		return;
	}

	if (current_node.type == TK_RETURN)
	{
		_advance(); // RETURN

		if (current_node.type == TYPE_EXPRESSION)
		{
			_generate_expression();
		}
		_append_line("  ret");
		return;
	}
	_advance();
}

void CodeGenerator::_generate_if_block()
{
	std::string end_if_label = "if_" + std::to_string(if_counter++);

	// keep track of current node id for nested conditionals
	unsigned int node_id = current_node.id;

	while (current_node.type == TK_IF || current_node.type == TK_ELSE)
	{
		unsigned int clause = if_clause_counter++;

		// check if its just the else
		if (current_node.type == TK_ELSE)
		{
			_advance(); // ELSE
		}

		if (current_node.type == TK_IF)
		{
			_advance(); // IF
			_generate_expression();

			_append_line("  test %eax,%eax");
			_append_line("  jz if_clause_" + std::to_string(clause));
			_advance(); // ;
			_advance(); // STATEMENT
		}

		_generate_statement();

		_append_line("  jz " + end_if_label);
		_append_line("if_clause_" + std::to_string(clause) + ":");

		if (current_node.type == TK_ELSE)
		{
			// check if we are nested.
			if (current_node.parent_id == node_id)
			{
				continue;
			}
		}
		else
		{
			_advance(); // end of if statment
		}

		_append_line(end_if_label + ":");

		// nested if therefore need to return.
		if (current_node.type == TK_ELSE)
		{
			return;
		}
	}
}

void CodeGenerator::_generate_expression()
{
	/*
	 * Use a stack based system
	 */
	int pushed_count = 0;
	while (current_node.type != TK_SEMICOLON)
	{
		_advance();
		if (current_node.type == TK_CONSTANT)
		{
			pushed_count++;
			_append_line("  pushl $" + current_node.value);
			continue;
		}

		_append_line("  popl %eax");
		if (pushed_count > 1)
		{
			_append_line("  popl %ebx");
		}

		switch (current_node.type)
		{
			case TK_PLUS:
			{
				_append_line("  addl %ebx,%eax");
			} break;
			case TK_STAR:
			{
				_append_line("  mull %ebx,%eax");
			} break;
		}

		_append_line("  pushl %eax");
	}
	_append_line("  popl %eax");
}

CodeGenerator::CodeGenerator()
{

}
