/*************************************************************************/
/*  parser.h                                                             */
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

#ifndef PARSER_H
#define PARSER_H

#include <set>
#include <string>
#include <memory>

#include "lexer.h"
#include "tokens.h"
#include "./data_structures/tree_node.h"

class Parser
{
public:
	std::set<Token> StorageClassSpecifiers
	{
		TK_TYPEDEF,
		TK_EXTERN,
		TK_STATIC,
		TK_AUTO,
		TK_REGISTER
	};

	std::set<Token> TypeSpecifiers
	{
		TK_VOID,
		TK_CHAR,
		TK_SHORT,
		TK_INT,
		TK_LONG,
		TK_FLOAT,
		TK_DOUBLE,
		TK_SIGNED,
		TK_UNSIGNED
	};

	std::set<Token> StructOrUnion
	{
		TK_STRUCT,
		TK_UNION
	};

	std::set<Token> TypeQualifiers
	{
		TK_CONST,
		TK_VOLATILE
	};

	std::set<Token> UnaryOperators
	{
		TK_BIT_AND,
		TK_BIT_NOT,
		TK_STAR,
		TK_PLUS,
		TK_MINUS,
		TK_NOT
	};

	std::set<Token> AssignmentOperators
	{
		TK_ASSIGN,
		TK_ASSIGN_MULTIPLICATION,
		TK_ASSIGN_DIVIDE,
		TK_ASSIGN_MODULO,
		TK_ASSIGN_PLUS,
		TK_ASSIGN_MINUS,
		TK_ASSIGN_BIT_SHIFT_LEFT,
		TK_ASSIGN_BIT_SHIFT_RIGHT,
		TK_ASSIGN_BIT_AND,
		TK_ASSIGN_BIT_OR,
		TK_ASSIGN_BIT_XOR,
	};

	struct Node
	{
		Token type;
		Token token;
		std::string value;
	};

private:
	Token current_token;
	Lexer lexer;
	std::string current_file;

	std::unique_ptr<TreeNode<Node>> _make_node(
			Token p_type,
			std::string p_value,
			Token p_token = NONE
	);

	void _update_node(
			std::unique_ptr<TreeNode<Node>> &p_node,
			Token p_type,
			std::string p_value,
			Token p_token = NONE
	);

	void _error(std::string p_error);

	void _print_tree(
			const std::unique_ptr<TreeNode<Node>> &p_current_node,
			bool p_last_child = true,
			std::string p_indent = ""
	);

	Token _peek();

	Token _get_next_token();
	void _advance();

	void _parse_program(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_external_declaration(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_function_definition(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_declaration_list(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_declaration_specifiers(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_declaration(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_init_declarator_list(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_init_declarator(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_declarator(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_pointer(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_type_qualifier_list(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_type_qualifier(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_direct_declarator(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool required = true
	);

	void _parse_compound_statment(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_block_item_list(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_statement(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_selection_statement(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_iteration_statement(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_jump_statement(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_assignment_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent,
			bool check_unary = false
	);

	void _parse_conditional_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_logical_or_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_logical_and_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_inclusive_or_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_exclusive_or_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_and_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_equality_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_relational_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_shift_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_additive_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_multiplicative_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_cast_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_unary_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_postfix_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

	void _parse_primary_expression(
			std::unique_ptr<TreeNode<Node>> &p_parent
	);

public:
	std::unique_ptr<TreeNode<Node>> parse(const std::string &p_file_path);

	Parser();
};

#endif // PARSER_H
