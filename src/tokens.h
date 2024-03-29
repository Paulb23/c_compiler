#ifndef TOKENS_H
#define TOKENS_H

#include <unordered_map>
#include <string>

enum Token
{
	/* misc */
	NONE,

	/* general tokens */
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
	TK_VARGS,
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
	TK_POST_INCREMENT,
	TK_POST_DECREMENT,

	TK_EOF,
	TK_ERROR,

	/* parser tokens */
	TYPE_PROGRAM,
	TYPE_EXTERNAL_DECLARATION,
	TYPE_FUNCTION_DECLARATION,
	TYPE_DECLARATION_SPECIFIER,
	TYPE_STORAGE_CLASS_SPECIFIER,
	TYPE_TYPE_SPECIFIER,
	TYPE_STRUCT_OR_UNION_SPECIFIER,
	TYPE_STRUCT_OR_UNION,
	TYPE_STRUCT_DECLARATION_LIST,
	TYPE_STRUCT_DECLARATION,
	TYPE_SPECIFIER_QUALIIFIER_LIST,
	TYPE_TYPE_QUALIFIER,
	TYPE_STRUCT_DECLARATOR_LIST,
	TYPE_STRUCT_DECLARATOR,
	TYPE_DECLARATOR,
	TYPE_POINTER,
	TYPE_QUALIFIER_LIST,
	TYPE_DIRECT_DECLARATOR,
	TYPE_ASSIGNMENT_EXPRESSION,
	TYPE_CONDITIONAL_EXPRESSION,
	TYPE_LOGICAL_OR_EXPRESSION,
	TYPE_LOGICAL_AND_EXPRESSION,
	TYPE_INCLUSIVE_OR_EXPRESSION,
	TYPE_INCLUSIVE_AND_EXPRESSION,
	TYPE_EXCLUSIVE_AND_EXPRESSION,
	TYPE_EXCLUSIVE_OR_EXPRESSION,
	TYPE_AND_EXPRESSION,
	TYPE_EQUALITY_EXPRESSION,
	TYPE_RELATIONAL_EXPRESSION,
	TYPE_SHIFT_EXPRESSION,
	TYPE_ADDITIVE_EXPRESSION,
	TYPE_MULTIPLICITIVE_EXPRESSION,
	TYPE_CAST_EXPRESSION,
	TYPE_UNARARY_EXPRESSION,
	TYPE_POSTFIX_EXPRESSION,
	TYPE_PRIMARY_EXPRESSION,
	TYPE_EXPRESSION,
	TYPE_ARGUMENT_EXPRESSION_LIST,
	TYPE_TYPE_NAME,
	TYPE_ABSTRACT_DECLARATOR,
	TYPE_DIRECT_ABSTRACT_DECLARATOR,
	TYPE_PARAMETER_TYPE_LIST,
	TYPE_PARAMETER_LIST,
	TYPE_PARAMETER_DECLARATION,
	TYPE_INITIALISER_LIST,
	TYPE_DESIGNATION,
	TYPE_DESIGNATOR_LIST,
	TYPE_DESIGNATOR,
	TYPE_CONSTANT_EXPRESSION,
	TYPE_INITIALISER,
	TYPE_UNARY_OPERATOR,
	TYPE_ASSIGNMENT_OPERATOR,
	TYPE_IDENTIFIER_LIST,
	TYPE_ENUM_SPECIFIER,
	TYPE_ENUMERATOR_LIST,
	TYPE_ENUMERATOR,
	TYPE_TYPEDEF_NAME,
	TYPE_FUNCTION_SPECIFIER,
	TYPE_DECLARATION_LIST,
	TYPE_DECLARATION,
	TYPE_INIT_DECLARATOR_LIST,
	TYPE_INIT_DECLARATOR,
	TYPE_COMPOUND_STATEMENT,
	TYPE_BLOCK_ITEM_LIST,
	TYPE_BLOCK_ITEM,
	TYPE_STATEMENT,
	TYPE_LABELED_STATEMENT,
	TYPE_EXPRESSION_STATMENT,
	TYPE_SELECTION_STATMENT,
	TYPE_ITERATION_STATMENT,
	TYPE_JUMP_STATMENT,

	TYPE_ENUMERATION_CONSTANT,
	TYPE_IDENTIFIER,
	TYPE_STRING_LITERAL,
	TYPE_CONSTANT,

	TYPE_UNKNOWN,

	/* Symatic analysis */
	FUNCTION,
	FUNCTION_CALL,
	DECLARATION,
	CODE_BLOCK,

	/* assembler tokens */
	OP_NONE,
	OP_BYTE,
	OP_SHORT,
	OP_WORD,
	OP_LONG,
	OP_QUAD,
	OP_TEN_BYTE,

	TK_GLOB,
	TK_LABEL,
	TK_MOV,
	TK_RET,
	TK_JMP,
	TK_CMP,
	TK_CALL,
	TK_SYSCALL,
	TK_TEST,

	TK_PUSH,
	TK_POP,

	TK_ADD,
	TK_SUB,
	TK_MUL,
	TK_INC,
	TK_DEC
};

const std::unordered_map<Token, std::string> token_to_string
{
	/* misc */
	{NONE, "NONE"},

	/* general tokens */
	{TK_VOID, "VOID"},
	{TK_CHAR, "CHAR"},
	{TK_INT, "INT"},
	{TK_FLOAT, "FLOAT"},
	{TK_DOUBLE, "DOUBLE"},
	{TK_LONG, "LONG"},
	{TK_SHORT, "SHORT"},
	{TK_SIGNED, "SIGNED"},
	{TK_UNSIGNED, "UNSIGNED"},
	{TK_AUTO, "AUTO"},
	{TK_EXTERN, "EXTERN"},
	{TK_STATIC, "STATIC"},
	{TK_REGISTER, "REGISTER"},
	{TK_VOLATILE, "VOLITILE"},
	{TK_CONST, "CONST"},
	{TK_ENUM, "ENUM"},
	{TK_TYPEDEF, "TYPEDEF"},
	{TK_UNION, "UNION"},
	{TK_STRUCT, "STRUCT"},
	{TK_GOTO, "GOTO"},
	{TK_IF, "IF"},
	{TK_ELSE, "ELSE"},
	{TK_SWITCH, "SWITCH"},
	{TK_CASE, "CASE"},
	{TK_DEFAULT, "DEFAULT"},
	{TK_FOR, "FOR"},
	{TK_WHILE, "WHILE"},
	{TK_DO, "DO"},
	{TK_CONTINUE, "CONTINUE"},
	{TK_BREAK, "BREAK"},
	{TK_RETURN, "RETURN"},
	{TK_SIZEOF, "SIZEOF"},
	{TK_NEWLINE, "NEWLINE"},
	{TK_CONSTANT, "CONSTANT"},
	{TK_IDENTIFIER, "IDENTIFIER"},
	{TK_BRACKET_OPEN, "BRACKET_OPEN"},
	{TK_BRACKET_CLOSE, "BRACKET_CLOSE"},
	{TK_BRACE_OPEN, "BRACE_OPEN"},
	{TK_BRACE_CLOSE, "BRACE_CLOSE"},
	{TK_PARENTHESIS_OPEN, "PARENTHESIS_OPEN"},
	{TK_PARENTHESIS_CLOSE, "PARENTHESIS_CLOSE"},
	{TK_SEMICOLON, "SEMICOLON"},
	{TK_COMMA, "COMMA"},
	{TK_DOT, "DOT"},
	{TK_VARGS, "..."},
	{TK_QUESION_MARK, "QUESTION_MARK"},
	{TK_COLON, "COLON"},
	{TK_BIT_NOT, "BIT_NOT"},
	{TK_BIT_AND, "BIT_AND"},
	{TK_BIT_OR, "BIT_OR"},
	{TK_BIT_XOR, "BIT_XOR"},
	{TK_BIT_SHIFT_LEFT, "BIT_SHIFT_LEFT"},
	{TK_BIT_SHIFT_RIGHT, "BIT_SHIFT_RIGHT"},
	{TK_STAR, "STAR"},
	{TK_DEREFERENCE, "DEREFERENCE"},
	{TK_PLUS, "PLUS"},
	{TK_MINUS, "MINUS"},
	{TK_DIVIDE, "DIVIDE"},
	{TK_MODULO, "MODULO"},
	{TK_ASSIGN, "ASSIGN"},
	{TK_ASSIGN_PLUS, "ASSIGN_PLUS"},
	{TK_ASSIGN_MINUS, "ASSING_MINUS"},
	{TK_ASSIGN_DIVIDE, "ASSIGN_DIVIDE"},
	{TK_ASSIGN_MODULO, "ASSIGN_MODULO"},
	{TK_ASSIGN_MULTIPLICATION, "ASSIGN_MULTIPLICATION"},
	{TK_ASSIGN_BIT_AND, "ASSIGN_BIT_AND"},
	{TK_ASSIGN_BIT_OR, "ASSIGN_BIT_OR"},
	{TK_ASSIGN_BIT_XOR, "ASSIGN_BIT_XOR"},
	{TK_ASSIGN_BIT_SHIFT_LEFT, "ASSIGN_BIT_SHIFT_LEFT"},
	{TK_ASSIGN_BIT_SHIFT_RIGHT, "ASSIGN_BIT_SHIFT_RIGHT"},
	{TK_NOT, "NOT"},
	{TK_AND, "AND"},
	{TK_OR, "OR"},
	{TK_NOT_EQUAL, "NOT_EQUAL"},
	{TK_EQUAL, "EQUAL"},
	{TK_GREATER_THAN, "GREATER_THAN"},
	{TK_GREATER_THAN_EQUAL, "GREATER_THAN_EQUAL"},
	{TK_LESS_THAN, "LESS_THAN"},
	{TK_LESS_THAN_EQUAL, "LESS_THAN_EQUAL"},
	{TK_INCREMENT, "INCREMENT"},
	{TK_DECREMENT, "DECREMENT"},
	{TK_POST_INCREMENT, "POST_INCREMENT"},
	{TK_POST_DECREMENT, "POST_DECREMENT"},
	{TK_EOF, "EOF"},
	{TK_ERROR, "ERROR"},

	/* parser tokens */
	{TYPE_PROGRAM, "PROGRAM"},
	{TYPE_EXTERNAL_DECLARATION, "EXTERNAL_DECLARATION"},
	{TYPE_FUNCTION_DECLARATION, "FUNC_DECLARATION"},
	{TYPE_DECLARATION_SPECIFIER, "DECLARATION_SPECIFIER"},
	{TYPE_STORAGE_CLASS_SPECIFIER, "STORAGE_CLASS_SPECIFIER"},
	{TYPE_TYPE_SPECIFIER, "TYPE_SPECIFIER"},
	{TYPE_STRUCT_OR_UNION_SPECIFIER, "STRUCT_OR_UNION_SPECIFIER"},
	{TYPE_STRUCT_OR_UNION, "STRUCT_OR_UNION"},
	{TYPE_STRUCT_DECLARATION_LIST, "STRUCT_DECLARATION_LIST"},
	{TYPE_STRUCT_DECLARATION, "STRUCT_DECLARATION"},
	{TYPE_SPECIFIER_QUALIIFIER_LIST, "SPECIFIER_QUALIIFIER_LIST"},
	{TYPE_TYPE_QUALIFIER, "TYPE_QUALIFIER"},
	{TYPE_STRUCT_DECLARATOR_LIST, "STRUCT_DECLARATOR_LIST"},
	{TYPE_STRUCT_DECLARATOR, "STRUCT_DECLARATOR"},
	{TYPE_DECLARATOR, "DECLARATOR"},
	{TYPE_POINTER, "POINTER"},
	{TYPE_QUALIFIER_LIST, "QUALIFIER_LIST"},
	{TYPE_DIRECT_DECLARATOR, "DIRECT_DECLARATOR"},
	{TYPE_ASSIGNMENT_EXPRESSION, "ASSIGNMENT_EXPRESSION"},
	{TYPE_CONDITIONAL_EXPRESSION, "CONDITIONAL_EXPRESSION"},
	{TYPE_LOGICAL_OR_EXPRESSION, "LOGICAL_OR_EXPRESSION"},
	{TYPE_LOGICAL_AND_EXPRESSION, "LOGICAL_AND_EXPRESSION"},
	{TYPE_INCLUSIVE_OR_EXPRESSION, "INCLUSIVE_OR_EXPRESSION"},
	{TYPE_INCLUSIVE_AND_EXPRESSION, "INCLUSIVE_AND_EXPRESSION"},
	{TYPE_EXCLUSIVE_AND_EXPRESSION, "EXCLUSIVE_AND_EXPRESSION"},
	{TYPE_EXCLUSIVE_OR_EXPRESSION, "EXCLUSIVE_OR_EXPRESSION"},
	{TYPE_AND_EXPRESSION, "AND_EXPRESSION"},
	{TYPE_EQUALITY_EXPRESSION, "EQUALITY_EXPRESSION"},
	{TYPE_RELATIONAL_EXPRESSION, "RELATIONAL_EXPRESSION"},
	{TYPE_SHIFT_EXPRESSION, "SHIFT_EXPRESSION"},
	{TYPE_ADDITIVE_EXPRESSION, "ADDITIVE_EXPRESSION"},
	{TYPE_MULTIPLICITIVE_EXPRESSION, "MULTIPLICITIVE_EXPRESSION"},
	{TYPE_CAST_EXPRESSION, "CAST_EXPRESSION"},
	{TYPE_UNARARY_EXPRESSION, "UNARARY_EXPRESSION"},
	{TYPE_POSTFIX_EXPRESSION, "POSTFIX_EXPRESSION"},
	{TYPE_PRIMARY_EXPRESSION, "PRIMARY_EXPRESSION"},
	{TYPE_EXPRESSION, "EXPRESSION"},
	{TYPE_ARGUMENT_EXPRESSION_LIST, "ARGUMENT_EXPRESSION_LIST"},
	{TYPE_TYPE_NAME, "TYPE_NAME"},
	{TYPE_ABSTRACT_DECLARATOR, "ABSTRACT_DECLARATOR"},
	{TYPE_DIRECT_ABSTRACT_DECLARATOR, "DIRECT_ABSTRACT_DECLARATOR"},
	{TYPE_PARAMETER_TYPE_LIST, "PARAMETER_TYPE_LIST"},
	{TYPE_PARAMETER_LIST, "PARAMETER_LIST"},
	{TYPE_PARAMETER_DECLARATION, "PARAMETER_DECLARATION"},
	{TYPE_INITIALISER_LIST, "INITIALISER_LIST"},
	{TYPE_DESIGNATION, "DESIGNATION"},
	{TYPE_DESIGNATOR_LIST, "DESIGNATOR_LIST"},
	{TYPE_DESIGNATOR, "DESIGNATOR"},
	{TYPE_CONSTANT_EXPRESSION, "CONSTANT_EXPRESSION"},
	{TYPE_INITIALISER, "INITIALISER"},
	{TYPE_UNARY_OPERATOR, "UNARY_OPERATOR"},
	{TYPE_ASSIGNMENT_OPERATOR, "ASSIGNMENT_OPERATOR"},
	{TYPE_IDENTIFIER_LIST, "IDENTIFIER_LIST"},
	{TYPE_ENUM_SPECIFIER, "ENUM_SPECIFIER"},
	{TYPE_ENUMERATOR_LIST, "ENUMERATOR_LIST"},
	{TYPE_ENUMERATOR, "ENUMERATOR"},
	{TYPE_TYPEDEF_NAME, "TYPEDEF_NAME"},
	{TYPE_FUNCTION_SPECIFIER, "FUNCTION_SPECIFIER"},
	{TYPE_DECLARATION_LIST, "DECLARATION_LIST"},
	{TYPE_DECLARATION, "DECLARATION"},
	{TYPE_INIT_DECLARATOR_LIST, "INIT_DECLARATOR_LIST"},
	{TYPE_DECLARATOR, "DECLARATOR"},
	{TYPE_COMPOUND_STATEMENT, "COMPOUND_STATEMENT"},
	{TYPE_BLOCK_ITEM_LIST, "BLOCK_ITEM_LIST"},
	{TYPE_BLOCK_ITEM, "BLOCK_ITEM"},
	{TYPE_STATEMENT, "STATEMENT"},
	{TYPE_LABELED_STATEMENT, "LABELED_STATEMENT"},
	{TYPE_EXPRESSION_STATMENT, "EXPRESSION_STATMENT"},
	{TYPE_SELECTION_STATMENT, "SELECTION_STATMENT"},
	{TYPE_ITERATION_STATMENT, "ITERATION_STATMENT"},
	{TYPE_JUMP_STATMENT, "JUMP_STATMENT"},

	{TYPE_ENUMERATION_CONSTANT, "ENUMERATION_CONSTANT"},
	{TYPE_IDENTIFIER, "IDENTIFIER"},
	{TYPE_STRING_LITERAL, "STRING_LITERAL"},
	{TYPE_CONSTANT, "CONSTANT"},

	{TYPE_UNKNOWN, ""},

	/* Symatic analysis */
	{FUNCTION, "FUNCTION"},
	{FUNCTION_CALL, "FUNCTION_CALL"},
	{DECLARATION, "DECLARATION"},
	{CODE_BLOCK, "CODE_BLOCK"},

	/* Assembeler */
	{ OP_NONE, "NONE"},
	{ OP_BYTE, "BYTE"},
	{ OP_SHORT, "SHORT"},
	{ OP_WORD, "WORD"},
	{ OP_LONG, "LONG"},
	{ OP_QUAD, "QUAD"},
	{ OP_TEN_BYTE, "TEN_BYTE"},

	{ TK_GLOB, "GLOB"},
	{ TK_LABEL, "LABEL"},
	{ TK_MOV, "MOV"},
	{ TK_RET, "RET"},
	{ TK_JMP, "JMP"},
	{ TK_CMP, "CMP"},
	{ TK_CALL, "CALL"},
	{ TK_SYSCALL, "SYS_CALL"},
	{ TK_TEST, "TEST"},

	{ TK_PUSH, "PUSH"},
	{ TK_POP, "POP"},

	{ TK_SUB, "SUB"},
	{ TK_ADD, "ADD"},
	{ TK_MUL, "MUL"},
	{ TK_INC, "INC"},
	{ TK_DEC, "DEC"}
};

const std::unordered_map<Token, int> op_precedence
{
	{TK_POST_INCREMENT, 1},
	{TK_POST_DECREMENT, 1},

	{TK_STAR, 3},
	{TK_DIVIDE, 3},
	{TK_MODULO, 3},

	{TK_PLUS, 4},
	{TK_MINUS, 4},

	{TK_LESS_THAN, 6},

	{TK_EQUAL, 7},

	{TK_ASSIGN, 14}
};

#endif // TOKENS_H
