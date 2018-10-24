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

void Parser::parse(const std::string &p_file_path)
{
	std::ifstream stream(p_file_path);
	if (!stream)
	{
		std::cout << "error: Cannot access " << p_file_path << std::endl;
		return;
	}
	lexer.clear();
	_make_root(p_file_path);

	const int buffer_size = 4096;
	std::unique_ptr<char[]> buffer(new char[buffer_size]);
	while (stream)
	{
		stream.read(buffer.get(), buffer_size);
		lexer.append_code(buffer.get());
		_parse_declaration_list();
	}
	stream.close();

	std::cout << "-----------------------------------------------" << std::endl;
	_print_tree(root);
	std::cout << "-----------------------------------------------" << std::endl;
}

void Parser::_make_root(std::string p_value)
{
	root.reset();
	Node program;
	program.type = TYPE_PROGRAM;
	program.value = p_value;
	root = std::unique_ptr<TreeNode<Node>>(new TreeNode<Node>());
	root->set_data(program);
}

std::unique_ptr<TreeNode<Parser::Node>> Parser::_make_node(Type p_type, std::string p_value)
{
	Node node;
	node.type = p_type;
	node.value = p_value;

	std::unique_ptr<TreeNode<Node>> tree_node(new TreeNode<Node>());
	tree_node->set_data(node);
	return tree_node;
}

void Parser::_error(std::string p_error)
{
	std::cout << root->get_data().value << ":" + (lexer.get_token_line() + 1) << ": error: " << p_error << std::endl;
	exit(0);
}


void Parser::_print_tree(const std::unique_ptr<TreeNode<Node>> &p_current_node, int p_depth)
{
	Node data = p_current_node->get_data();

	std::string indent = std::string(p_depth, ' ' );
	std::cout << indent << "+-" << type_to_string.at(data.type) << " " << data.value << std::endl;

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

Lexer::Token Parser::_get_next_token()
{
	Lexer::Token token = lexer.get_token();
	while (token != Lexer::TK_EOF && token != Lexer::TK_ERROR)
	{
		token = lexer.advance();
		if (token != Lexer::TK_NEWLINE) {
			break;
		}
	}
	return token;
}

void Parser::_advance()
{
	current_token = _get_next_token();
	if (current_token == Lexer::TK_ERROR)
	{
		_error("cannot reconise symbol '" + lexer.get_token_value() + "'");
	}
}

Lexer::Token Parser::_peek()
{
	return lexer.peek();
}

/*
 * Grammer rules start here, based on:
 * https://slebok.github.io/zoo/c/c99/iso-9899-tc3/extracted/index.html
*/

void Parser::_parse_declaration_list()
{
	std::unique_ptr<TreeNode<Node>> current_node = _make_node(TYPE_UNKNOWN, "");
	_advance();
	while (current_token != Lexer::TK_EOF && current_token != Lexer::TK_ERROR)
	{
		if (DeclarationSpecifiers.count(current_token) == 0)
		{
			_error("expected declaration, but found: '" + lexer.get_token_value() + "'");
		}

		_parse_declaration_specifiers(current_node);
		switch (current_token)
		{
			case Lexer::TK_IDENTIFIER:
			{
				Node data = current_node->get_data();
				data.type = TYPE_FUNCTION;
				data.value = lexer.get_token_value();
				current_node->set_data(data);

				_advance();
				_parse_function(current_node); // assume function for now...
				_advance();
			};
			case Lexer::TK_STRUCT:
			case Lexer::TK_UNION:
			{
				//_parse_union_or_struct();
			} break;
			case Lexer::TK_ENUM:
			{
				//_parse_enum();
				std::cout << lexer.get_token_value() << std::endl;
			} break;
			default:
			{
			} break;
		}
		_advance();
	}
	root->add_child(current_node);
}

void Parser::_parse_declaration_specifiers(std::unique_ptr<TreeNode<Node>> &p_current_node)
{
	if (DeclarationSpecifiers.count(current_token) == 0)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> current_node = _make_node(TYPE_DECLARATION_SPECIFIER, lexer.get_token_value());
	p_current_node->add_child(current_node);

	_advance();
	_parse_declaration_specifiers(p_current_node);
}

void Parser::_parse_function(std::unique_ptr<TreeNode<Node>> &p_current_node)
{
	if (current_token != Lexer::TK_PARENTHESIS_OPEN)
	{
		_error("error: expected '(', but found: '" + lexer.get_token_value() + "'");
	}

	// handle arguments;
	_advance();
	if (current_token != Lexer::TK_PARENTHESIS_CLOSE)
	{
		_error("error: expected ')', but found: '" + lexer.get_token_value() + "'");
	}

	_advance();
	if (current_token != Lexer::TK_BRACE_OPEN)
	{
		_error("error: expected '{', but found: '" + lexer.get_token_value() + "'");
	}

	std::unique_ptr<TreeNode<Node>> current_node = _make_node(TYPE_CODE_BLOCK, "code_block");
	while (true)
	{
		_advance();
		if (current_token == Lexer::TK_EOF)
		{
			_error("end of file found expected '}'");
		}

		if (current_token == Lexer::TK_RETURN)
		{
			std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_RETURN, lexer.get_token_value());

			_parse_unary_expression(node);
			_advance();

			current_node->add_child(node);

			if (current_token != Lexer::TK_SEMICOLON)
			{
				_error("end of statment found, expected ';' but received: '" + lexer.get_token_value() + "'");
			}
		}

		if (current_token == Lexer::TK_BRACE_CLOSE)
		{
			break;
		}
	}
	p_current_node->add_child(current_node);
}

void Parser::_parse_unary_expression(std::unique_ptr<TreeNode<Node>> &p_current_node)
{
	_advance();
	if (current_token == Lexer::TK_CONSTANT)
	{
		std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_CONSTANT, lexer.get_token_value());
		p_current_node->add_child(node);
		return;
	}

	if (UnaryOperators.count(current_token))
	{
		std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_UNARY_OP, lexer.get_token_value());
		p_current_node->add_child(node);
	}

	_parse_unary_expression(p_current_node);
}

Parser::Parser()
{

}
