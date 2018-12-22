/*************************************************************************/
/*  symantic_analysier.cpp                                               */
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

#include "symantic_analysier.h"

#include <iostream>
#include <stack>
#include <queue>

std::unique_ptr<TreeNode<SymanticAnalysier::Node>> SymanticAnalysier::analyise(
		const std::unique_ptr<TreeNode<Parser::Node>> &parse_tree
) {

	current_node_offset = -1;
	tree_vector = _create_list(parse_tree);
	_advance();

	if (current_node.type != TYPE_PROGRAM)
	{
		_error("expected program, but found: '" + token_to_string.at(current_node.type) + "'");
	}

	std::unique_ptr<TreeNode<Node>> root = _make_node(TYPE_PROGRAM, current_node.value);

	_advance();
	while (current_node.type == TYPE_EXTERNAL_DECLARATION)
	{
		std::unique_ptr<TreeNode<Node>> node = NULL;
		_advance();
		switch (current_node.type)
		{
			case TYPE_FUNCTION_DECLARATION:
			{
				node = _make_node(FUNCTION, "");
				_analyse_function_declaration(node);
			} break;
		}

		if (node == NULL)
		{
			break;
		}
		root->add_child(node);
	}

	std::cout << "-----------------------------------------------" << std::endl;
	_print_tree(root);
	std::cout << "-----------------------------------------------" << std::endl;

	return root;
}

std::vector<Parser::Node> SymanticAnalysier::_create_list(
		const std::unique_ptr<TreeNode<Parser::Node>> &p_root
) {
	std::vector<Parser::Node> node_list;
	node_list.push_back(p_root->get_data());

	const std::list<std::unique_ptr<TreeNode<Parser::Node>>> &children = p_root->get_children();
	for (const std::unique_ptr<TreeNode<Parser::Node>> &child : children)
	{
		std::vector<Parser::Node> child_nodes = _create_list(child);
		for (Parser::Node &childs : child_nodes)
		{
			node_list.push_back(childs);
		}
	}
	return node_list;
}

std::unique_ptr<TreeNode<SymanticAnalysier::Node>> SymanticAnalysier::_make_node(
		Token p_type,
		std::string p_value
) {
	Node node;
	node.type = p_type;
	node.value = p_value;

	std::unique_ptr<TreeNode<Node>> tree_node(new TreeNode<Node>());
	tree_node->set_data(node);
	return tree_node;
}

void SymanticAnalysier::_update_node(
		std::unique_ptr<TreeNode<Node>> &p_node,
		Token p_type,
		std::string p_value
) {
	Node node = p_node->get_data();
	node.type = p_type;
	node.value = p_value;
	p_node->set_data(node);
}

void SymanticAnalysier::_error(std::string p_error)
{
	std::cout << "error: " << p_error << std::endl;
	exit(0);
}


void SymanticAnalysier::_print_tree(
		const std::unique_ptr<TreeNode<Node>> &p_current_node,
		bool p_last_child,
		std::string p_indent
) {
	Node data = p_current_node->get_data();

	std::cout << p_indent << std::ends;
	if (p_last_child)
	{
		std::cout << " └─" << std::ends;
		p_indent += "    ";
	}
	else
	{
		std::cout << " ├─" << std::ends;
		p_indent += " | ";
	}
	std::cout << token_to_string.at(data.type) << " " << data.value << std::endl;

	const std::list<std::unique_ptr<TreeNode<Node>>> &children = p_current_node->get_children();

	for (const std::unique_ptr<TreeNode<Node>> &child : children)
	{
		_print_tree(child, child == children.back(), p_indent);
	}
}

void SymanticAnalysier::_advance()
{
	if (current_node_offset + 1 > tree_vector.size())
	{
		return;
	}
	current_node_offset++;
	current_node = tree_vector[current_node_offset];
}

/*
 * Analysis starts here.
 */
void SymanticAnalysier::_analyse_function_declaration(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {

	/* skip return types for now */
	while (current_node.type != TYPE_IDENTIFIER) { _advance(); }

	_update_node(p_parent, FUNCTION, current_node.value);

	/* skip function args */
	while (current_node.type != TYPE_COMPOUND_STATEMENT) { _advance(); }

	std::unique_ptr<TreeNode<Node>> code_block = _make_node(CODE_BLOCK, current_node.value);
	_advance();
	_analyse_code_block(code_block);
	p_parent->add_child(code_block);
}

void SymanticAnalysier::_analyse_code_block(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	if (current_node.type != TK_BRACE_OPEN)
	{
		_error("expected '{', but found: '"+ token_to_string.at(current_node.type) +"'");
	}
	_advance();

	while (current_node.type == TYPE_BLOCK_ITEM_LIST)
	{
		/* hack this in for the mean time */
		_advance();
		while (current_node.type == TYPE_STATEMENT)
		{
			switch (current_node.type)
			{
			    case TYPE_STATEMENT:
			    {
				    _analyse_statement(p_parent);
			    } break;
			}
		}
	}

	if (current_node.type == TK_BRACE_CLOSE)
	{
		return;
	}
}

void SymanticAnalysier::_analyse_statement(
        std::unique_ptr<TreeNode<Node>> &p_parent
) {
	_advance();
	_advance();
	switch (current_node.type)
	{
	    case TK_BRACE_OPEN: // compound statment work around for now...
	    {
		    _advance(); // {
			_analyse_statement(p_parent);
			_advance(); // }
	    } break;
	    case TK_IF:
	    {
		    std::unique_ptr<TreeNode<Node>> if_node = _make_node(TK_IF, current_node.value);

			_advance(); // if
			_advance(); // (
			_analyse_expression(if_node);

			std::unique_ptr<TreeNode<Node>> statment = _make_node(TYPE_STATEMENT, current_node.value);
			_analyse_statement(statment);
			if_node->add_child(statment);

			if (current_node.type == TK_ELSE)
			{
				std::unique_ptr<TreeNode<Node>> else_node = _make_node(TK_ELSE, current_node.value);
				_advance(); // else

				_analyse_statement(else_node);
				if_node->add_child(else_node);
			}

			p_parent->add_child(if_node);
	    } break;
	    case TK_WHILE:
	    {
		    std::unique_ptr<TreeNode<Node>> while_node = _make_node(TK_WHILE, current_node.value);

			_advance(); // while
			_advance(); // (
			_analyse_expression(while_node);

			std::unique_ptr<TreeNode<Node>> statment = _make_node(TYPE_STATEMENT, current_node.value);
			_analyse_statement(statment);
			while_node->add_child(statment);

			p_parent->add_child(while_node);
	    } break;
	    case TK_DO:
	    {
		    std::unique_ptr<TreeNode<Node>> do_node = _make_node(TK_DO, current_node.value);

			_advance(); // do

			std::unique_ptr<TreeNode<Node>> statment = _make_node(TYPE_STATEMENT, current_node.value);
			_analyse_statement(statment);
			do_node->add_child(statment);

			std::unique_ptr<TreeNode<Node>> while_node = _make_node(TK_WHILE, current_node.value);

			_advance(); // while
			_advance(); // (
			_analyse_expression(while_node);
			do_node->add_child(while_node);

			p_parent->add_child(do_node);
	    } break;
	    case TK_FOR:
	    {
		    std::unique_ptr<TreeNode<Node>> for_node = _make_node(TK_FOR, current_node.value);

			_advance(); // for
			_advance(); // (

			_analyse_expression(for_node);
			_analyse_expression(for_node);
			_analyse_expression(for_node);

			std::unique_ptr<TreeNode<Node>> statment = _make_node(TYPE_STATEMENT, current_node.value);
			_analyse_statement(statment);
			for_node->add_child(statment);

			p_parent->add_child(for_node);
	    } break;
	    case TK_BREAK:
	    {
		    std::unique_ptr<TreeNode<Node>> break_node = _make_node(TK_BREAK, current_node.value);
			p_parent->add_child(break_node);

			_advance(); // break
			_advance(); // ;
	    } break;
	    case TK_CONTINUE:
	    {
		    std::unique_ptr<TreeNode<Node>> continue_node = _make_node(TK_CONTINUE, current_node.value);
			p_parent->add_child(continue_node);

			_advance(); // continue
			_advance(); // ;
	    } break;
	    case TK_GOTO:
	    {
		    std::unique_ptr<TreeNode<Node>> goto_node = _make_node(TK_GOTO, current_node.value);

			_advance(); // goto

			std::unique_ptr<TreeNode<Node>> identifier_node = _make_node(TYPE_IDENTIFIER, current_node.value);
			goto_node->add_child(identifier_node);

			_advance(); // identifier
			_advance(); // ;

			p_parent->add_child(goto_node);

	    } break;
	    case TK_RETURN:
	    {
		    std::unique_ptr<TreeNode<Node>> return_node = _make_node(TK_RETURN, current_node.value);

			_advance(); // return
			_analyse_expression(return_node);

			p_parent->add_child(return_node);
	    } break;
	}
}


void SymanticAnalysier::_analyse_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	/*
	 * Use shunting yard to convert infix to postfix.
	 *
	 * However we need a queue instead of a stack as
	 * the nodes are added to the tree in reverse order
	 */
	std::queue<Parser::Node> output_queue;
	std::stack<Parser::Node> op_stack;

	while (current_node.type != TK_SEMICOLON && current_node.type != TYPE_STATEMENT)
	{
		/* constants and basic operators for now */
		if (current_node.token == TK_CONSTANT)
		{
			output_queue.push(current_node);
			_advance();
			continue;
		}

		if (!op_precedence.count(current_node.token))
		{
			_advance();
			continue;
		}

		if (op_stack.empty())
		{
			op_stack.push(current_node);
			_advance();
			continue;
		}

		while (!op_stack.empty())
		{
			if (op_precedence.at(op_stack.top().token) < op_precedence.at(current_node.token))
			{
				output_queue.push(op_stack.top());
				op_stack.pop();
			}
			else
			{
				break;
			}
		}
		op_stack.push(current_node);
		_advance();
	}

	while (!op_stack.empty())
	{
		output_queue.push(op_stack.top());
		op_stack.pop();
	}

	/*
	 * Build the tree
	 */
	std::unique_ptr<TreeNode<Node>> expression = _make_node(TYPE_EXPRESSION, "");
	while (!output_queue.empty())
	{
		Parser::Node top = output_queue.front();
		std::unique_ptr<TreeNode<Node>> child = _make_node(top.token, top.value);
		output_queue.pop();
		expression->add_child(child);
	}
	std::unique_ptr<TreeNode<Node>> semi_colon = _make_node(TK_SEMICOLON, ";");
	expression->add_child(semi_colon);
	p_parent->add_child(expression);

	if (current_node.type == TK_SEMICOLON)
	{
		_advance();
	}
}

SymanticAnalysier::SymanticAnalysier()
{

}
