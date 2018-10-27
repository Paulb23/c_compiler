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

#include "tokens.h"
#include <unordered_map>
#include <string>

class Lexer
{
public:
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
