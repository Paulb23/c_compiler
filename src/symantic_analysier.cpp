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
		switch (current_node.type)
		{
			case TYPE_STATEMENT:
			{
				_advance();
				_advance();
				switch (current_node.type)
				{
					case TK_RETURN:
					{
						std::unique_ptr<TreeNode<Node>> return_node = _make_node(TK_RETURN, current_node.value);

						_advance();
						if (current_node.type == TYPE_CONSTANT)
						{
							std::unique_ptr<TreeNode<Node>> constant = _make_node(TYPE_CONSTANT, current_node.value);
							return_node->add_child(constant);
						}
						_advance();
						p_parent->add_child(return_node);
					} break;
				}
			} break;
		}

	}

	if (current_node.type == TK_BRACE_CLOSE)
	{
		return;
	}
}


SymanticAnalysier::SymanticAnalysier()
{

}
