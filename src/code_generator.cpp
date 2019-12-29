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
	function_map.clear();

	if_counter = 0;
	if_clause_counter = 0;
	loop_counter = 0;
	comp_clause_counter = 0;

	code.clear();
	current_node_offset = -1;
	tree_vector = _create_list(p_root);
	_advance();

	if (current_node.type != TYPE_PROGRAM)
	{
		_error("expected program, but found: '" + token_to_string.at(current_node.type) + "'");
	}

	_advance();

	/* Inject _start */
	_append_line("globl _start");
	_append_line("_start:");
	_append_line("  push %ebp");
	_append_line("  movl %esp,%ebp");
	_append_line("  call main");
	_append_line("  movl %eax,%edi");
	_append_line("  push $60");
	_append_line("  popl %eax");
	_append_line("  syscall");
	_append_line("  ret"); /* debug only, not executed. */

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

void CodeGenerator::_warn(std::string p_warning)
{
	std::cout << "warning: " << p_warning << std::endl;
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
	while (current_node.type == FUNCTION)
	{
		_generate_function();
	}
}

void CodeGenerator::_generate_function()
{
	_append_line("globl " + current_node.value);
	_append_line(current_node.value + ":");
	function_map[current_node.value] = 0;

	_advance();

	// set up stack frame for this function
	_append_line("  push %ebp");
	_append_line("  movl %esp,%ebp");

	Scope scope;
	scope.stack_offset = -16;
	while (current_node.type == TYPE_IDENTIFIER)
	{
		Var var;
		var.scope_level = scope.level;
		var.stack_offset = scope.stack_offset;
		scope.var_map[current_node.value] = var;
		scope.stack_offset -= 8;
		_advance();
	}
	scope.stack_offset = 8;

	if (current_node.type == CODE_BLOCK)
	{
		_generate_code_block(scope);
	}
}

void CodeGenerator::_generate_code_block(const Scope &p_scope)
{
	_advance();
	Scope scope = p_scope;
	scope.level += 1;
	while (
		   current_node.type != FUNCTION &&
		   current_node.type != TK_BRACE_CLOSE &&
		   current_node_offset + 1 < tree_vector.size())
	{
		if (current_node.type == DECLARATION)
		{
			scope = _generate_declaration(scope);
			continue;
		}

		if (current_node.type == TYPE_ASSIGNMENT_EXPRESSION)
		{
			_generate_assignment_expression(scope);
			continue;
		}

		_generate_statement(scope);
	}

	if (current_node.type != FUNCTION)
	{
		_advance();
	}
}

CodeGenerator::Scope CodeGenerator::_generate_declaration(const Scope &p_scope)
{
	_advance();

	Scope scope = p_scope;
	int vars = 0;
	while (current_node.type == TK_IDENTIFIER)
	{
		if (
			scope.var_map.count(current_node.value) &&
			scope.var_map[current_node.value].scope_level == scope.level
		) {
			_error(current_node.value + " is already defined.");
		}
		Var var;
		var.scope_level = scope.level;
		var.stack_offset = scope.stack_offset;
		scope.var_map[current_node.value] = var;
		scope.stack_offset += 8;
		vars++;
		_advance();
	}

	if (current_node.type == TYPE_EXPRESSION)
	{
		_generate_expression(scope);
	}

	for (int i = 0; i < vars; i++)
	{
		_append_line("  pushl %eax");
	}

	return scope;
}

void CodeGenerator::_generate_assignment_expression(const Scope &p_scope)
{
	_advance();

	std::string lvalue = current_node.value;

	_advance();
	_generate_expression(p_scope);

	int offset = p_scope.var_map.at(lvalue).stack_offset;
	_append_line("  movl %eax,-" + std::to_string(offset) + "(%ebp)");
}

void CodeGenerator::_generate_statement(const Scope &p_scope)
{
	if (current_node.type == TK_BRACE_OPEN)
	{
		_generate_code_block(p_scope);
		return;
	}

	if (current_node.type == TK_IF)
	{
		_generate_if_block(p_scope);
		return;
	}

	if (current_node.type == TK_WHILE)
	{
		unsigned int loop = loop_counter++;

		_advance(); // WHILE

		_append_line("loop_start_" + std::to_string(loop) + ":");
		_generate_expression(p_scope);
		_append_line("  test %eax,%eax");
		_append_line("  jz loop_end_" + std::to_string(loop));

		_advance(); // ;
		_advance(); // STATEMENT

		if (current_node.type == TK_BRACE_OPEN)
		{
			_generate_code_block(p_scope);
		}
		else
		{
			_generate_statement(p_scope);
		}

		_append_line("  jmp loop_start_" + std::to_string(loop));
		_append_line("loop_end_" + std::to_string(loop) + ":");
		return;
	}

	if (current_node.type == TK_FOR)
	{
		unsigned int loop = loop_counter++;

		_advance(); // FOR

		 // loop init
		if (_peek() != TK_SEMICOLON)
		{
			_generate_expression(p_scope);
		}
		else
		{
			_advance(); // EXPRESSION
		}
		_advance(); // ;

		_append_line("loop_start_" + std::to_string(loop) + ":");
		// loop condition, if empty make infinate
		if (_peek() != TK_SEMICOLON)
		{
			_generate_expression(p_scope);
		}
		else
		{
			_advance();
			_append_line("  pushl $1");
			_append_line("  popl %eax");
		}
		_append_line("  test %eax,%eax");
		_append_line("  jz loop_end_" + std::to_string(loop));
		_advance(); // ;

		_advance(); // STATEMENT

		if (current_node.type == TK_BRACE_OPEN)
		{
			_generate_code_block(p_scope);
		}
		else
		{
			_generate_statement(p_scope);
		}

		// loop post
		_advance(); // EXPRESSION
		if (_peek() != TK_SEMICOLON)
		{
			_generate_expression(p_scope);
		}
		else
		{
			_advance(); // EXPRESSION
		}
		_advance(); // ;

		_append_line("  jmp loop_start_" + std::to_string(loop));
		_append_line("loop_end_" + std::to_string(loop) + ":");
		return;
	}

	if (current_node.type == TK_DO)
	{
		unsigned int loop = loop_counter++;

		_advance(); // DO
		_advance(); // STATEMENT

		_append_line("loop_start_" + std::to_string(loop) + ":");
		if (current_node.type == TK_BRACE_OPEN)
		{
			_generate_code_block(p_scope);
		}
		else
		{
			_generate_statement(p_scope);
		}
		_advance(); // WHILE
		_generate_expression(p_scope);
		_append_line("  test %eax,%eax");
		_append_line("  jz loop_end_" + std::to_string(loop));
		_advance(); // ;

		_append_line("  jmp loop_start_" + std::to_string(loop));
		_append_line("loop_end_" + std::to_string(loop) + ":");
		return;
	}

	if (current_node.type == TK_RETURN)
	{
		_advance(); // RETURN

		if (current_node.type == TYPE_EXPRESSION)
		{
			_generate_expression(p_scope);
		}
		// restore stack frame
		_append_line("  movl %ebp,%esp");
		_append_line("  pop %ebp");
		_append_line("  ret");
		return;
	}
	_advance();
}

void CodeGenerator::_generate_if_block(const Scope &p_scope)
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
			_generate_expression(p_scope);

			_append_line("  test %eax,%eax");
			_append_line("  jz if_clause_" + std::to_string(clause));
			_advance(); // ;
			_advance(); // STATEMENT
		}

		if (current_node.type == TK_BRACE_OPEN)
		{
			_generate_code_block(p_scope);
		}
		else
		{
			_generate_statement(p_scope);
		}

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

		_append_line(end_if_label + ":");

		// nested if therefore need to return.
		if (current_node.type == TK_ELSE)
		{
			return;
		}
	}
}

void CodeGenerator::_generate_expression(const Scope &p_scope)
{
	/*
	 * Use a stack based system
	 */
	int pushed_count = 0;
	std::string previous = "";
	while (current_node.type != TK_SEMICOLON)
	{
		_advance();
		if (current_node.type == TK_SEMICOLON)
		{
			break;
		}

		if (current_node.type == TK_CONSTANT)
		{
			pushed_count++;
			_append_line("  pushl $" + current_node.value);
			continue;
		}

		if (current_node.type == FUNCTION_CALL)
		{
			std::string function_name = current_node.value;
			int arg_count = 0;
			if (_peek() == TYPE_ARGUMENT_EXPRESSION_LIST)
			{
				_advance(); // call
				_advance(); // arg expression
				_advance(); // (
				while (current_node.type == TYPE_EXPRESSION)
				{
					arg_count++;
					_generate_expression(p_scope);
					_advance(); // ;
					if (_peek() == TYPE_EXPRESSION)
					{
						_advance(); // )
					}
					_append_line("  pushl %eax");
				}
			}
			_append_line("  call " + function_name);
			/* remove args, can be improved with add to esp */
			while (arg_count > 0)
			{
				_append_line("  popl %ecx");
				arg_count--;
			}
			_append_line("  pushl %eax"); /* TODO return is in EAX so this can be removed. */
			pushed_count++;
			continue;
		}

		if (current_node.type == TK_IDENTIFIER)
		{
			if (!p_scope.var_map.count(current_node.value))
			{
				_error(current_node.value + " is not defined.");
			}
			pushed_count++;
			int offset = p_scope.var_map.at(current_node.value).stack_offset * -1;
			_append_line("  movl " + std::to_string(offset) + "(%ebp),%eax");
			_append_line("  pushl %eax");
			previous = current_node.value;
			continue;
		}

		_append_line("  popl %eax");
		if (pushed_count > 1)
		{
			_append_line("  popl %ebx");
		}

		switch (current_node.type)
		{
			case TK_ASSIGN:
			{
				int offset = p_scope.var_map.at(previous).stack_offset;
				_append_line("  movl %eax,-" + std::to_string(offset) + "(%ebp)");
			} break;
			case TK_PLUS:
			{
				_append_line("  addl %ebx,%eax");
			} break;
			case TK_MINUS:
			{
				/* this is a hack - TODO: refactor */
				_append_line("  subl %eax,%ebx");
				_append_line("  pushl %ebx");
				_append_line("  popl %eax");
			} break;
			case TK_EQUAL:
			{
				/* this is a hack - TODO: refactor */
				_append_line("  cmp %ebx,%eax");
				_append_line("  jz comp_clause_eq_" + std::to_string(comp_clause_counter));
				_append_line("  pushl $0");
				_append_line("  popl %eax");
				_append_line("  jmp comp_clause_end_" + std::to_string(comp_clause_counter));
				_append_line("comp_clause_eq_" + std::to_string(comp_clause_counter) + ":");
				_append_line("  pushl $1");
				_append_line("  popl %eax");
				_append_line("comp_clause_end_" + std::to_string(comp_clause_counter) + ":");
				comp_clause_counter++;
			} break;
			case TK_LESS_THAN:
			{
				/* ditto. */
				_append_line("  cmp %ebx,%eax");
				_append_line("  jle comp_clause_eq_" + std::to_string(comp_clause_counter));
				_append_line("  pushl $1");
				_append_line("  popl %eax");
				_append_line("  jmp comp_clause_end_" + std::to_string(comp_clause_counter));
				_append_line("comp_clause_eq_" + std::to_string(comp_clause_counter) + ":");
				_append_line("  pushl $0");
				_append_line("  popl %eax");
				_append_line("comp_clause_end_" + std::to_string(comp_clause_counter) + ":");
				comp_clause_counter++;
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
