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

	if (current_token != TK_SEMICOLON)
	{
		std::unique_ptr<TreeNode<Node>> expression = _make_node(TYPE_EXPRESSION, "");
		_parse_expression(expression);
		p_parent->add_child(expression);
	}

	if (current_token != TK_SEMICOLON)
	{
		_error("expected ';' but found: '" + lexer.get_token_value() + "'");
	}
	node = _make_node(TK_SEMICOLON, lexer.get_token_value(), TK_SEMICOLON);
	p_parent->add_child(node);
	_advance();
}

void Parser::_parse_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {

	std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_ASSIGNMENT_EXPRESSION, "");
	_parse_assignment_expression(node);
	p_parent->add_child(node);

	if (current_token != TK_COMMA)
	{
		return;
	}
	std::unique_ptr<TreeNode<Node>> comma = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(comma);
	_advance();

	_parse_expression(p_parent);
}

void Parser::_parse_assignment_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	//TODO: unary expression
	std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_CONDITIONAL_EXPRESSION, "");
	_parse_conditional_expression(node);

	p_parent->add_child(node);
}

void Parser::_parse_conditional_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> logical_or = _make_node(TYPE_LOGICAL_OR_EXPRESSION, "");
	_parse_logical_or_expression(logical_or);
	p_parent->add_child(logical_or);

	if (current_token != TK_QUESION_MARK)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> question_mark = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(question_mark);
	_advance();

	std::unique_ptr<TreeNode<Node>> expression = _make_node(TYPE_EXPRESSION, "");
	_parse_expression(expression);
	p_parent->add_child(expression);

	if (current_token != TK_COLON)
	{
		_error("expected ':' but found '" + lexer.get_token_value() + "'");
	}

	std::unique_ptr<TreeNode<Node>> colon = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(colon);
	_advance();

	std::unique_ptr<TreeNode<Node>> next = _make_node(TYPE_CONDITIONAL_EXPRESSION, "");
	_parse_conditional_expression(next);
	p_parent->add_child(next);
}

void Parser::_parse_logical_or_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> logical_and = _make_node(TYPE_LOGICAL_AND_EXPRESSION, "");
	_parse_logical_and_expression(logical_and);
	p_parent->add_child(logical_and);

	if (current_token != TK_OR)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> logic_or = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(logic_or);
	_advance();

	_parse_logical_or_expression(p_parent);
}

void Parser::_parse_logical_and_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> inclusive_or = _make_node(TYPE_INCLUSIVE_OR_EXPRESSION, "");
	_parse_inclusive_or_expression(inclusive_or);
	p_parent->add_child(inclusive_or);

	if (current_token != TK_AND)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> logic_and = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(logic_and);
	_advance();

	_parse_logical_and_expression(p_parent);
}

void Parser::_parse_inclusive_or_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> exclusive_or = _make_node(TYPE_EXCLUSIVE_OR_EXPRESSION, "");
	_parse_exclusive_or_expression(exclusive_or);
	p_parent->add_child(exclusive_or);

	if (current_token != TK_BIT_OR)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> bit_or = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(bit_or);
	_advance();

	_parse_inclusive_or_expression(p_parent);
}

void Parser::_parse_exclusive_or_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> and_expression = _make_node(TYPE_AND_EXPRESSION, "");
	_parse_and_expression(and_expression);
	p_parent->add_child(and_expression);

	if (current_token != TK_BIT_XOR)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> bit_xor = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(bit_xor);
	_advance();

	_parse_exclusive_or_expression(p_parent);
}

void Parser::_parse_and_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> equality_expression = _make_node(TYPE_EQUALITY_EXPRESSION, "");
	_parse_equality_expression(equality_expression);
	p_parent->add_child(equality_expression);

	if (current_token != TK_BIT_AND)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> bit_and = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(bit_and);
	_advance();

	_parse_and_expression(p_parent);
}

void Parser::_parse_equality_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> relational_expression = _make_node(TYPE_RELATIONAL_EXPRESSION, "");
	_parse_relational_expression(relational_expression);
	p_parent->add_child(relational_expression);

	if (current_token != TK_EQUAL && current_token != TK_NOT_EQUAL)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> equals = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(equals);
	_advance();

	_parse_equality_expression(p_parent);
}

void Parser::_parse_relational_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> shift_expression = _make_node(TYPE_SHIFT_EXPRESSION, "");
	_parse_shift_expression(shift_expression);
	p_parent->add_child(shift_expression);

	if (
		current_token != TK_LESS_THAN && current_token != TK_LESS_THAN_EQUAL &&
		current_token != TK_GREATER_THAN && current_token != TK_GREATER_THAN_EQUAL
	) {
		return;
	}

	std::unique_ptr<TreeNode<Node>> relational = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(relational);
	_advance();

	_parse_relational_expression(p_parent);
}

void Parser::_parse_shift_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> additive_expression = _make_node(TYPE_ADDITIVE_EXPRESSION, "");
	_parse_additive_expression(additive_expression);
	p_parent->add_child(additive_expression);

	if (current_token != TK_BIT_SHIFT_LEFT && current_token != TK_ASSIGN_BIT_SHIFT_RIGHT)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> shift = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(shift);
	_advance();

	_parse_shift_expression(p_parent);
}

void Parser::_parse_additive_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> multi_expression = _make_node(TYPE_MULTIPLICITIVE_EXPRESSION, "");
	_parse_multiplicative_expression(multi_expression);
	p_parent->add_child(multi_expression);

	if (current_token != TK_PLUS && current_token != TK_MINUS)
	{
		return;
	}

	std::unique_ptr<TreeNode<Node>> additive = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(additive);
	_advance();

	_parse_additive_expression(p_parent);
}

void Parser::_parse_multiplicative_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> cast_expression = _make_node(TYPE_CAST_EXPRESSION, "");
	_parse_cast_expression(cast_expression);
	p_parent->add_child(cast_expression);

	if (
			current_token != TK_STAR &&
			current_token != TK_DIVIDE &&
			current_token != TK_MODULO
	) {
		return;
	}

	std::unique_ptr<TreeNode<Node>> multiplicative = _make_node(
				current_token,
				lexer.get_token_value(),
				current_token
	);
	p_parent->add_child(multiplicative);
	_advance();

	_parse_multiplicative_expression(p_parent);
}

void Parser::_parse_cast_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
		// TODO: add casting
		std::unique_ptr<TreeNode<Node>> node = _make_node(TYPE_UNARARY_EXPRESSION, "");
		_parse_unary_expression(node);
		p_parent->add_child(node);
}

void Parser::_parse_unary_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	// TODO:
	// ++
	// --
	// sizeof
	if (UnaryOperators.count(current_token))
	{
		std::unique_ptr<TreeNode<Node>> unary_op = _make_node(
					TYPE_UNARY_OPERATOR,
					lexer.get_token_value(),
					lexer.get_token()
		);
		p_parent->add_child(unary_op);
		_advance();

		std::unique_ptr<TreeNode<Node>> cast_expression = _make_node(TYPE_CAST_EXPRESSION, "");
		_parse_cast_expression(cast_expression);
		p_parent->add_child(cast_expression);
		return;
	}

	std::unique_ptr<TreeNode<Node>> primary_expression = _make_node(TYPE_POSTFIX_EXPRESSION, "");
	_parse_postfix_expression(primary_expression);
	p_parent->add_child(primary_expression);
}

void Parser::_parse_postfix_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	std::unique_ptr<TreeNode<Node>> primary_expression = _make_node(TYPE_PRIMARY_EXPRESSION, "");
	_parse_primary_expression(primary_expression);
	p_parent->add_child(primary_expression);
}

void Parser::_parse_primary_expression(
		std::unique_ptr<TreeNode<Node>> &p_parent
) {
	switch (current_token) {
		case TK_IDENTIFIER:
		{
			std::unique_ptr<TreeNode<Node>> indentifier = _make_node(
						TYPE_IDENTIFIER,
						lexer.get_token_value(),
						TK_IDENTIFIER
			);
			p_parent->add_child(indentifier);
			_advance();
			return;
		} break;
		case TK_CONSTANT:
		{
			std::unique_ptr<TreeNode<Node>> constant = _make_node(
						TYPE_CONSTANT,
						lexer.get_token_value(),
						TK_CONSTANT
			);
			p_parent->add_child(constant);
			_advance();
			return;
		} break;
		/* TODO: strings */
		case TK_PARENTHESIS_OPEN:
		{
			std::unique_ptr<TreeNode<Node>> open_parenthesis = _make_node(
						TK_PARENTHESIS_OPEN,
						lexer.get_token_value(),
						TK_PARENTHESIS_OPEN
			);
			p_parent->add_child(open_parenthesis);
			_advance();

			std::unique_ptr<TreeNode<Node>> expression = _make_node(TYPE_EXPRESSION, "");
			_parse_expression(expression);
			p_parent->add_child(expression);

			if (current_token != TK_PARENTHESIS_CLOSE)
			{
				_error("expected ')' but found: '" + lexer.get_token_value() + "'");
			}

			std::unique_ptr<TreeNode<Node>> close_parenthesis = _make_node(
						TK_PARENTHESIS_CLOSE,
						lexer.get_token_value(),
						TK_PARENTHESIS_CLOSE
			);
			p_parent->add_child(close_parenthesis);
			_advance();
			return;
		} break;
	}

	_error("expected primary expression but found: '" + token_to_string.at(current_token) + "'");
}

Parser::Parser()
{

}
