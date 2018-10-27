/*************************************************************************/
/*  parser.cpp                                                           */
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

#include "parser.h"

#include <iostream>
#include <fstream>

std::unique_ptr<TreeNode<Parser::Node>> Parser::parse(const std::string &p_file_path)
{
	std::ifstream stream(p_file_path);
	if (!stream)
	{
		std::cout << "error: Cannot access " << p_file_path << std::endl;
		return NULL;
	}
	lexer.clear();

	current_file = p_file_path;
	std::unique_ptr<TreeNode<Node>> root = _make_node(TYPE_PROGRAM, p_file_path);

	const int buffer_size = 4096;
	std::unique_ptr<char[]> buffer(new char[buffer_size]);
	while (stream)
	{
		stream.read(buffer.get(), buffer_size);
		lexer.append_code(buffer.get());
		_parse_program(root);
	}
	stream.close();

	std::cout << "-----------------------------------------------" << std::endl;
	_print_tree(root);
	std::cout << "-----------------------------------------------" << std::endl;

	return root;
}

std::unique_ptr<TreeNode<Parser::Node>> Parser::_make_node(
		Token p_type,
		std::string p_value,
		Token p_token
) {
	Node node;
	node.type = p_type;
	node.token = p_token;
	node.value = p_value;

	std::unique_ptr<TreeNode<Node>> tree_node(new TreeNode<Node>());
	tree_node->set_data(node);
	return tree_node;
}

void Parser::_update_node(
		std::unique_ptr<TreeNode<Node>> &p_node,
		Token p_type,
		std::string p_value,
		Token p_token
) {
	Node node = p_node->get_data();
	node.type = p_type;
	node.value = p_value;
	node.token = p_token;
	p_node->set_data(node);
}

void Parser::_error(std::string p_error)
{
	std::cout << current_file << ": line " << (lexer.get_token_line() + 1);
	std::cout << ": error: " << p_error << std::endl;
	exit(0);
}


void Parser::_print_tree(
		const std::unique_ptr<TreeNode<Node>> &p_current_node,
		int p_depth
) {
	Node data = p_current_node->get_data();

	std::string indent = std::string(p_depth, ' ' );
	std::cout << indent << "+-";
	std::cout << token_to_string.at(data.type) << " " << data.value << std::endl;

	int depth = p_depth + 2;

	const std::list<std::unique_ptr<TreeNode<Node>>> &children = p_current_node->get_children();

	for (const std::unique_ptr<TreeNode<Node>> &child : children)
	{
		_print_tree(child, depth);
		if (child != children.back())
		{
			std::cout << indent << "  |" << std::endl;
		}
	}
}

Token Parser::_get_next_token()
{
	Token token = lexer.get_token();
	while (token != TK_EOF && token != TK_ERROR)
	{
		token = lexer.advance();
		if (token != TK_NEWLINE) {
			break;
		}
	}
	return token;
}

void Parser::_advance()
{
	current_token = _get_next_token();
	if (current_token == TK_ERROR)
	{
		_error("cannot reconise symbol '" + lexer.get_token_value() + "'");
	}
}

Token Parser::_peek()
{
	return lexer.peek();
}

/*
 * Grammer rules start here, based on:
 * https://slebok.github.io/zoo/c/c99/iso-9899-tc3/extracted/index.html
*/

void Parser::_parse_program(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	_parse_external_declaration(p_parent);
}

void Parser::_parse_external_declaration(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> current_node = _make_node(TYPE_EXTERNAL_DECLARATION, "");

	_advance();

	/* assume function, need better look ahead. */
	std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_FUNCTION_DECLARATION, "");
	_parse_function_definition(node);

	current_node->add_child(node);
	p_parent->add_child(current_node);
}

void Parser::_parse_function_definition(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> declaration_specifiers = _make_node(TYPE_DECLARATION_SPECIFIER, "");
	_parse_declaration_specifiers(declaration_specifiers);
	p_parent->add_child(declaration_specifiers);

	std::unique_ptr<TreeNode<Node>> declarators = _make_node(TYPE_DECLARATOR, "");
	_parse_declarator(declarators);
	p_parent->add_child(declarators);

	// parse optional list?

	std::unique_ptr<TreeNode<Node>> compound_statement = _make_node(TYPE_COMPOUND_STATEMENT, "");
	_parse_compound_statment(compound_statement);
	p_parent->add_child(compound_statement);
}

void Parser::_parse_declaration_specifiers(
		std::unique_ptr<TreeNode<Node>> &p_parent,
		bool top_level
) {
	std::unique_ptr<TreeNode<Node>> node = NULL;
	if (StorageClassSpecifiers.count(current_token))
	{
		node = _make_node(
			TYPE_STORAGE_CLASS_SPECIFIER,
			lexer.get_token_value(),
			current_token
		);
	}
	else if (TypeSpecifiers.count(current_token))
	{
		node = _make_node(
			TYPE_TYPE_SPECIFIER,
			lexer.get_token_value(),
			current_token
		);
		/*
		 * TODO: struct-or-union, enum-specifier, typedef-name
		 */
	}
	else if (TypeQualifiers.count(current_token))
	{
		node = _make_node(
			TYPE_TYPE_QUALIFIER,
			lexer.get_token_value(),
			current_token
		);
	}
	/*
	 * TODO: function-specifier
	 */

	if (node == NULL)
	{
		if (top_level)
		{
			_error("expected declaration specifiers, but found none.");
		}
		return;
	}
	p_parent->add_child(node);

	_advance();
	_parse_declaration_specifiers(p_parent, false);
}

void Parser::_parse_declarator(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	if (current_token == TK_STAR)
	{
		//TODO: _parse_pointer();
	}

	std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_DIRECT_DECLARATOR, "");
	_parse_direct_declarator(node);
	p_parent->add_child(node);
}

void Parser::_parse_direct_declarator(
		std::unique_ptr<TreeNode<Node>> &p_parent,
		bool required
) {
	std::unique_ptr<TreeNode<Node>> node = NULL;
	if (current_token == TK_IDENTIFIER)
	{
		node = _make_node(TYPE_IDENTIFIER, lexer.get_token_value(), TK_IDENTIFIER);
	}

	// function args only for now.
	if (current_token == TK_PARENTHESIS_OPEN)
	{
		node = _make_node(TK_PARENTHESIS_OPEN, lexer.get_token_value(), TK_PARENTHESIS_OPEN);
		// TODO: parse argument list here.
		p_parent->add_child(node);

		_advance();
		if (current_token != TK_PARENTHESIS_CLOSE)
		{
			_error("expected ')' but found: '" + lexer.get_token_value() + "'");
		}
		node = _make_node(TK_PARENTHESIS_CLOSE, lexer.get_token_value(), TK_PARENTHESIS_CLOSE);
	}

	if (node == NULL)
	{
		if (required)
		{
			_error("expected identifier, but found none.");
		}
		return;
	}
	p_parent->add_child(node);

	_advance();
	_parse_direct_declarator(p_parent, false);
}

void Parser::_parse_compound_statment(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> node = NULL;
	if (current_token != TK_BRACE_OPEN)
	{
		_error("expected '{' but found: '" + lexer.get_token_value() + "'");
	}
	node = _make_node(TK_BRACE_OPEN, lexer.get_token_value(), TK_BRACE_OPEN);
	p_parent->add_child(node);
	_advance();

	node = _make_node(TYPE_BLOCK_ITEM_LIST, "");
	_parse_block_item_list(node);
	p_parent->add_child(node);

	if (current_token != TK_BRACE_CLOSE)
	{
		_error("expected '}' but found: '" + lexer.get_token_value() + "'");
	}
	node = _make_node(TK_BRACE_CLOSE, lexer.get_token_value(), TK_BRACE_CLOSE);
	p_parent->add_child(node);
	_advance();
}

void Parser::_parse_block_item_list(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	// just parse statments for now...
	std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_STATEMENT, "");;
	_parse_statement(node);
	p_parent->add_child(node);
}

void Parser::_parse_statement(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	// just jump statments..
	std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_JUMP_STATMENT, "");;
	_parse_jump_statement(node);
	p_parent->add_child(node);
}

void Parser::_parse_jump_statement(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> node = NULL;
	// just returns...
	if (current_token == TK_RETURN)
	{
		node = _make_node(TK_RETURN, lexer.get_token_value(), TK_RETURN);
		p_parent->add_child(node);
		_advance();
	}

	// asume there's a conststant for now.
	if (current_token == TK_CONSTANT)
	{
		node = _make_node(TYPE_CONSTANT, lexer.get_token_value(), TK_CONSTANT);
		p_parent->add_child(node);
		_advance();
	}

	if (current_token != TK_SEMICOLON)
	{
		_error("expected ';' but found: '" + lexer.get_token_value() + "'");
	}
	node = _make_node(TK_SEMICOLON, lexer.get_token_value(), TK_SEMICOLON);
	p_parent->add_child(node);
	_advance();
}

Parser::Parser()
{

}
