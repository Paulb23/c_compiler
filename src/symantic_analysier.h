/*************************************************************************/
/*  symantic_analysier.h                                                 */
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

#ifndef SYMANTIC_ANALYSIER_H
#define SYMANTIC_ANALYSIER_H

#include <string>
#include <memory>
#include <vector>

#include "parser.h"
#include "tokens.h"
#include "./data_structures/tree_node.h"

class SymanticAnalysier
{
public:

	struct Node
	{
		Token type;
		std::string value;
	};
private:

	std::vector<Parser::Node> _create_list(
			const std::unique_ptr<TreeNode<Parser::Node>> &p_root
	);

	std::unique_ptr<TreeNode<Node>> _make_node(
			Token p_type,
			std::string p_value
	);

	void _update_node(
			std::unique_ptr<TreeNode<Node>> &p_node,
			Token p_type,
			std::string p_value
	);

	void _error(std::string p_error);

	void _print_tree(
			const std::unique_ptr<TreeNode<Node>> &p_current_node,
			bool p_last_child = true,
			std::string p_indent = ""
	);


	unsigned int current_node_offset;
	std::vector<Parser::Node> tree_vector;
	Parser::Node current_node;

	void _advance();

	void _analyse_function_declaration(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _analyse_code_block(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);


public:
	std::unique_ptr<TreeNode<Node>> analyise(
			const std::unique_ptr<TreeNode<Parser::Node>> &parse_tree
	);

	SymanticAnalysier();
};

#endif // SYMANTIC_ANALYSIER_H
