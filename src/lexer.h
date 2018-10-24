/*************************************************************************/
/*  lexer.h                                                              */
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

#ifndef LEXER_H
#define LEXER_H

#include <unordered_map>
#include <string>

class Lexer
{
public:
	enum Token
	{
		TK_VOID,
		TK_CHAR,
		TK_INT,
		TK_FLOAT,
		TK_DOUBLE,

		TK_LONG,
		TK_SHORT,
		TK_SIGNED,
		TK_UNSIGNED,

		TK_AUTO,
		TK_EXTERN,
		TK_STATIC,
		TK_REGISTER,

		TK_VOLATILE,
		TK_CONST,

		TK_ENUM,
		TK_TYPEDEF,
		TK_UNION,
		TK_STRUCT,

		TK_GOTO,
		TK_IF,
		TK_ELSE,
		TK_SWITCH,
		TK_CASE,
		TK_DEFAULT,

		TK_FOR,
		TK_WHILE,
		TK_DO,

		TK_CONTINUE,
		TK_BREAK,
		TK_RETURN,

		TK_SIZEOF,

		TK_NEWLINE,

		TK_CONSTANT,
		TK_IDENTIFIER,

		TK_BRACKET_OPEN,
		TK_BRACKET_CLOSE,
		TK_BRACE_OPEN,
		TK_BRACE_CLOSE,
		TK_PARENTHESIS_OPEN,
		TK_PARENTHESIS_CLOSE,
		TK_SEMICOLON,
		TK_COMMA,
		TK_DOT,
		TK_QUESION_MARK,
		TK_COLON,

		TK_BIT_NOT,
		TK_BIT_AND,
		TK_BIT_OR,
		TK_BIT_XOR,
		TK_BIT_SHIFT_LEFT,
		TK_BIT_SHIFT_RIGHT,

		TK_STAR,
		TK_DEREFERENCE,

		TK_PLUS,
		TK_MINUS,
		TK_DIVIDE,
		TK_MODULO,
		TK_ASSIGN,
		TK_ASSIGN_PLUS,
		TK_ASSIGN_MINUS,
		TK_ASSIGN_DIVIDE,
		TK_ASSIGN_MODULO,
		TK_ASSIGN_MULTIPLICATION,
		TK_ASSIGN_BIT_AND,
		TK_ASSIGN_BIT_OR,
		TK_ASSIGN_BIT_XOR,
		TK_ASSIGN_BIT_SHIFT_LEFT,
		TK_ASSIGN_BIT_SHIFT_RIGHT,

		TK_NOT,
		TK_AND,
		TK_OR,

		TK_NOT_EQUAL,
		TK_EQUAL,
		TK_GREATER_THAN,
		TK_GREATER_THAN_EQUAL,
		TK_LESS_THAN,
		TK_LESS_THAN_EQUAL,

		TK_INCREMENT,
		TK_DECREMENT,

		TK_EOF,
		TK_ERROR
	};

	const std::unordered_map<std::string, Token> keyword_map
	{
		{"void", TK_VOID},
		{"char", TK_CHAR},
		{"int", TK_INT},
		{"float", TK_FLOAT},
		{"double", TK_DOUBLE},

		{"long", TK_LONG},
		{"short", TK_SHORT},
		{"signed", TK_SIGNED},
		{"unsigned", TK_UNSIGNED},

		{"auto", TK_AUTO},
		{"extern", TK_EXTERN},
		{"static", TK_STATIC},
		{"register", TK_REGISTER},

		{"volatile", TK_VOLATILE},
		{"const", TK_CONST},

		{"typedef", TK_TYPEDEF},
		{"enum", TK_ENUM},
		{"union", TK_UNION},
		{"struct", TK_STRUCT},

		{"goto", TK_GOTO},
		{"if", TK_IF},
		{"else", TK_ELSE},
		{"switch", TK_SWITCH},
		{"case", TK_CASE},
		{"default", TK_DEFAULT},

		{"for", TK_FOR},
		{"while", TK_WHILE},
		{"do", TK_DO},

		{"continue", TK_CONTINUE},
		{"break", TK_BREAK},
		{"return", TK_RETURN},

		{"sizeof", TK_SIZEOF}
	};

private:
	struct _token_data
	{
		Token token;
		std::string value;
		int line;
	} token_data;
	Token _push_token(Token p_token, std::string p_value);

	int code_size;
	int offset;
	std::string code;

	int line;
	int column;

	char _get_next_char();
	char _look_ahead(const int p_amount) const;
public:
	void clear();
	void append_code(const std::string &p_code);

	Token advance();
	Token peek();

	Token get_token() const;
	std::string get_token_value() const;
	int get_token_line() const;

	Lexer();
};

#endif // LEXER_H
