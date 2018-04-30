/*************************************************************************/
/*  code_generator.cpp                                                   */
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


#include "code_generator.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "./data_structures/tree_node.h"

void CodeGenerator::generate_code(std::unique_ptr<TreeNode<Parser::Node>> &p_root, const std::string &p_output_file)
{
	std::string generated_code = "";
	std::vector<Parser::Node> node_list = _create_list(p_root);

	int i = 0;
	while (i < node_list.size())
	{
		Parser::Node node = node_list[i];

		switch (node.type)
		{
			case Parser::TYPE_FUNCTION:
			{
				generated_code += ".globl " + node.value + "\n" + node.value + ":\n";
			} break;
			case Parser::TYPE_CONSTANT:
			{
				generated_code += "  movl $" + node.value + "," + _get_register(THRITY_TWO_BITS) + "\n";
			} break;
			case Parser::TYPE_RETURN:
			{
				i++;
				node = node_list[i];
				switch (node.type)
				{
					case Parser::TYPE_CONSTANT:
					{
						generated_code += "  movl $" + node.value + "," + _get_register(THRITY_TWO_BITS) + "\n";
					} break;
				}
				generated_code += "  ret\n";
			} break;
			default:
			{
			}break;
		}
		i++;
	}
	std::cout << generated_code << std::endl;
	std::ofstream file;
	file.open(p_output_file);
	file << generated_code;
	file.close();
}

std::vector<Parser::Node> CodeGenerator::_create_list(const std::unique_ptr<TreeNode<Parser::Node>> &p_current_node)
{
	std::vector<Parser::Node> node_list;
	node_list.push_back(p_current_node->get_data());

	const std::list<std::unique_ptr<TreeNode<Parser::Node>>> &children = p_current_node->get_children();
	for (const std::unique_ptr<TreeNode<Parser::Node>> &child : children)
	{
		std::vector<Parser::Node> child_nodes = _create_list(child);
		for (Parser::Node &childs : child_nodes)
		{
			node_list.push_back(childs);
		}
	}
	return node_list;
}

std::string CodeGenerator::_get_register(Bits p_bits)
{
	switch (p_bits) {
	case EIGHT_BITS:
	{
		return eight_bit_register[0];
	} break;
	case SIXTEEN_BITS:
	{
		return sixteen_bit_register[0];
	} break;
	case THRITY_TWO_BITS:
	{
		return thrity_two_bit_register[0];
	} break;
	case SIXTY_FOUR_BITS:
	{
		return sixteen_bit_register[0];
	} break;
	default:
		break;
	}
	return eight_bit_register[0];
}

CodeGenerator::CodeGenerator()
{

}
