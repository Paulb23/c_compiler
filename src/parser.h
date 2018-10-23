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
#include "./data_structures/tree_node.h"

class Parser
{
public:
	enum Type
	{
		TYPE_PROGRAM,
		TYPE_UNKNOWN,
		TYPE_DECLARATION_SPECIFIER,
		TYPE_FUNCTION,
		TYPE_CODE_BLOCK,
		TYPE_RETURN,
		TYPE_CONSTANT,
		TYPE_UNARY_OP
	};

	const std::unordered_map<Type, std::string> type_to_string
	{
		{TYPE_PROGRAM,"PROGRAM"},
		{TYPE_UNKNOWN,"UNKOWN"},
		{TYPE_DECLARATION_SPECIFIER, "DEC_SPEC"},
		{TYPE_FUNCTION, "FUNCTION"},
		{TYPE_CODE_BLOCK, "CODE_BLOCK"},
		{TYPE_RETURN, "RETURN"},
		{TYPE_CONSTANT, "CONST"},
		{TYPE_UNARY_OP, "UNARY_OP"}
	};

	std::set<Lexer::Token> DeclarationSpecifiers
	{
		Lexer::TK_TYPEDEF,
		Lexer::TK_EXTERN,
		Lexer::TK_STATIC,
		Lexer::TK_AUTO,
		Lexer::TK_REGISTER,
		Lexer::TK_VOID,
		Lexer::TK_CHAR,
		Lexer::TK_SHORT,
		Lexer::TK_INT,
		Lexer::TK_LONG,
		Lexer::TK_FLOAT,
		Lexer::TK_DOUBLE,
		Lexer::TK_SIGNED,
		Lexer::TK_UNSIGNED,
		Lexer::TK_CONST,
		Lexer::TK_VOLATILE
	};

	std::set<Lexer::Token> UnaryOperators
	{
		Lexer::TK_BIT_AND,
		Lexer::TK_BIT_NOT,
		Lexer::TK_STAR,
		Lexer::TK_PLUS,
		Lexer::TK_MINUS,
		Lexer::TK_NOT
	};

	struct Node
	{
		Type type;
		std::string value;
	};

	std::unique_ptr<TreeNode<Node>> root;

private:
	Lexer::Token current_token;
	Lexer lexer;

	void _make_root(std::string p_value);

	std::unique_ptr<TreeNode<Node>> _make_node(Type p_type, std::string p_value);
	void _error(std::string p_error);

	void _print_tree(const std::unique_ptr<TreeNode<Node>> &p_current_node, int p_depth = 0);

	Lexer::Token _get_next_token();
	void _advance();

	void _parse_declaration_list();
	void _parse_declaration_specifiers(std::unique_ptr<TreeNode<Node>> &p_current_node);

	void _parse_function(std::unique_ptr<TreeNode<Node>> &p_current_node);

	void _parse_unary_expression(std::unique_ptr<TreeNode<Node>> &p_current_node);
public:
	void parse(const std::string &p_file_path);

	Parser();
};

#endif // PARSER_H
