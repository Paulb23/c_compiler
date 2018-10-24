/*************************************************************************/
/*  lexer.cpp                                                            */
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

#include <iostream>
#include "lexer.h"

static bool _is_number(const char &c)
{
	return (c >= '0' && c <= '9');
}

static bool _is_text_char(const char &c)
{
	return  (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			_is_number(c)          ||
			c == '_';
}

void Lexer::clear()
{
	code = "";
	line = 0;
	column = 0;
	offset = -1;
	code_size = 0;
}

void Lexer::append_code(const std::string &p_code)
{
	code = p_code;
	offset = -1;
	code_size = p_code.length();
}

Lexer::Token Lexer::peek()
{
	int current_line = line;
	int current_column = column;
	int current_offset = offset;

	Token token = advance();

	line = current_line;
	column = current_column;
	offset = current_offset;

	return token;
}

Lexer::Token Lexer::advance()
{
	while (true)
	{
		const char &c = _get_next_char();
		switch(c)
		{
			case 0:
			{
				return _push_token(TK_EOF, "eof");
			} break;

			case '\r':
				if (_look_ahead(1) != '\n')
				{
					return _push_token(TK_ERROR, "expected new line.");
				}
				_get_next_char();
				// fallthrough to new line
			case '\n':
				line++;
				return _push_token(TK_NEWLINE, "\n");

			// whitespace
			case ' ':
			case '\t':
				continue;

			case '{':
			{
				return _push_token(TK_BRACE_OPEN, "{");
			} break;
			case '}':
			{
				return _push_token(TK_BRACE_CLOSE, "}");
			} break;
			case '(':
			{
				return _push_token(TK_PARENTHESIS_OPEN, "(");
			} break;
			case ')':
			{
				return _push_token(TK_PARENTHESIS_CLOSE, ")");
			} break;
			case '[':
			{
				return _push_token(TK_BRACKET_OPEN, "]");
			} break;
			case ']':
			{
				return _push_token(TK_BRACKET_CLOSE, "]");
			} break;
			case ';':
			{
				return _push_token(TK_SEMICOLON, ";");
			} break;
			case ',':
			{
				return _push_token(TK_COMMA, ",");
			} break;
			case '?':
			{
				return _push_token(TK_QUESION_MARK, "?");
			} break;
			case ':':
			{
				return _push_token(TK_COLON, ":");
			} break;
			case '*':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_MULTIPLICATION, "*=");
				}
				return _push_token(TK_STAR, "*");
			} break;
			case '%':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_MODULO, "%=");
				}
				return _push_token(TK_MODULO, "%");
			} break;
			case '/':
			{
				if (_look_ahead(1) == '/')
				{
					_get_next_char();
					while (true)
					{
						const char &cc = _get_next_char();
						if (cc == '0' || cc == '\r' || cc == '\n')
						{
							break;
						}
					}
					offset--;
					column--;
					continue;
				}

				if (_look_ahead(1) == '*')
				{
					_get_next_char();
					while (true)
					{
						const char &cc = _get_next_char();
						if (cc == '\n' || (cc == '\r' && _look_ahead(1) == '\n'))
						{
							line++;
						}

						if (cc == '0' || (cc == '*' && _look_ahead(1) == '/'))
						{
							_get_next_char();
							break;
						}
					}
					continue;
				}

				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_DIVIDE, "/=");
				}
				_push_token(TK_DIVIDE, "/");
			} break;
			case '+':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_PLUS, "+=");
				}

				if (_look_ahead(1) == '+')
				{
					_get_next_char();
					return _push_token(TK_INCREMENT, "++");
				}
				return _push_token(TK_PLUS, "+");
			} break;
			case '-':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_MINUS, "-=");
				}

				if (_look_ahead(1) == '-')
				{
					_get_next_char();
					return _push_token(TK_DECREMENT, "--");
				}

				if (_look_ahead(1) == '>')
				{
					_get_next_char();
					return _push_token(TK_DEREFERENCE, "->");
				}
				return _push_token(TK_MINUS, "-");
			} break;
			case '>':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_GREATER_THAN_EQUAL, ">=");
				}

				if (_look_ahead(1) == '>')
				{
					_get_next_char();
					if (_look_ahead(1) == '=')
					{
						_get_next_char();
						return _push_token(TK_ASSIGN_BIT_SHIFT_RIGHT, ">>=");
					}
					return _push_token(TK_BIT_SHIFT_RIGHT, ">>");
				}
				return _push_token(TK_GREATER_THAN, ">");
			} break;
			case '<':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_LESS_THAN_EQUAL, "<=");
				}

				if (_look_ahead(1) == '<')
				{
					_get_next_char();
					if (_look_ahead(1) == '=')
					{
						_get_next_char();
						return _push_token(TK_ASSIGN_BIT_SHIFT_LEFT, "<<=");
					}
					return _push_token(TK_BIT_SHIFT_LEFT, "<<");
				}
				return _push_token(TK_LESS_THAN, "<");
			} break;
			case '!':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_NOT_EQUAL, "!=");
				}
				return _push_token(TK_NOT, "!");
			} break;
			case '^':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_BIT_XOR, "^=");
				}
				return _push_token(TK_BIT_XOR, "^");
			} break;
			case '~':
			{
				return _push_token(TK_BIT_NOT, "~");
			} break;
			case '|':
			{
				if (_look_ahead(1) == '|')
				{
					_get_next_char();
					return _push_token(TK_OR, "||");
				}

				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_BIT_OR, "|=");
				}
				return _push_token(TK_BIT_OR, "|");
			} break;
			case '&':
			{
				if (_look_ahead(1) == '&')
				{
					_get_next_char();
					return _push_token(TK_AND, "&&");
				}

				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_ASSIGN_BIT_AND, "&=");
				}
				return _push_token(TK_BIT_AND, "&");
			} break;
			case '=':
			{
				if (_look_ahead(1) == '=')
				{
					_get_next_char();
					return _push_token(TK_EQUAL, "==");
				}
				return _push_token(TK_ASSIGN, "=");
			} break;
			default:
			{
				if (_is_number(c))
				{
					int i = 0;
					std::string number;
					while (_is_number(_look_ahead(i)))
					{
						number += _look_ahead(i);
						i++;
					}
					offset += i - 1;
					column += i - 1;
					return _push_token(TK_CONSTANT, number);
				}

				if (c == '.')
				{
					return _push_token(TK_DOT, ".");
				}

				int i = 0;
				std::string word;
				while (_is_text_char(_look_ahead(i)))
				{
					word += _look_ahead(i);
					i++;
				}
				offset += i - 1;
				column += i - 1;

				if (keyword_map.count(word) > 0)
				{
					return _push_token(keyword_map.at(word), word);
				}
				return _push_token(TK_IDENTIFIER, word);
			} break;
		}
		return _push_token(TK_ERROR, "unkown token.");
	}
}

Lexer::Token Lexer::_push_token(Token p_token, std::string p_value)
{
	token_data.token = p_token;
	token_data.value = p_value;
	token_data.line = line;
	return p_token;
}

Lexer::Token Lexer::get_token() const
{
	return token_data.token;
}

std::string Lexer::get_token_value() const
{
	return token_data.value;
}

int Lexer::get_token_line() const
{
	return token_data.line;
}

char Lexer::_get_next_char()
{
	column++;
	offset++;
	return _look_ahead(0);
}

char Lexer::_look_ahead(const int p_amount) const
{
	if (offset + p_amount >= code_size)
	{
		return 0;
	}
	return code[offset + p_amount];
}

Lexer::Lexer()
{
	clear();
}
