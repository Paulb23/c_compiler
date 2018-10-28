/*************************************************************************/
/*  assembler.cpp                                                        */
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

#include "assembler.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <elf.h>

#define HEX( x ) setw(2) << setfill('0') << hex << (int)( x )

void Assembler::assemble(const std::string &p_input_file, const std::string &p_output_file)
{
	_generate_header();
	_generate_program_header();

	_generate_text(p_input_file);

	text_program_header.p_filesz = sizeof(text);
	text_program_header.p_memsz = sizeof(text);

	std::ofstream file;
	file.open(p_output_file);

	file.write((char *)&header, sizeof(Elf64_Ehdr));
	file.write((char *)&text_program_header, sizeof(Elf64_Phdr));

	for(unsigned int i=0; i< text.size(); ++i)
	{
		std::cout << std::hex << (int)text[i];
	}

	for (const char opcode : text)
	{
		file.write((const char *)&opcode, 1);
	}

	file.close();
}

void Assembler::_generate_header()
{
	header.e_ident[EI_MAG0] = ELFMAG0;
	header.e_ident[EI_MAG1] = ELFMAG1;
	header.e_ident[EI_MAG2] = ELFMAG2;
	header.e_ident[EI_MAG3] = ELFMAG3;
	header.e_ident[EI_CLASS] = ELFCLASS64;
	header.e_ident[EI_DATA] = ELFDATA2LSB;
	header.e_ident[EI_VERSION] = EV_CURRENT;
	header.e_ident[EI_OSABI] = ELFOSABI_NONE;
	header.e_ident[EI_ABIVERSION] = 0;
	header.e_ident[EI_PAD] = 0;

	header.e_type = ET_EXEC;
	header.e_machine = EM_X86_64;
	header.e_version = EV_CURRENT;
	header.e_entry = TEXT_ADDR;
	header.e_phoff = sizeof(Elf64_Ehdr);
	header.e_shoff = 0;
	header.e_flags = 0;
	header.e_ehsize = sizeof(Elf64_Ehdr);
	header.e_phentsize = sizeof(Elf64_Phdr);
	header.e_phnum = 1;
	header.e_shentsize = sizeof(Elf64_Shdr);
	header.e_shnum = 0;
	header.e_shstrndx = 0;
}

void Assembler::_generate_program_header()
{
	text_program_header.p_type = PT_LOAD;
	text_program_header.p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
	text_program_header.p_vaddr = TEXT_ADDR;
	text_program_header.p_paddr = 0;
	text_program_header.p_filesz = 0;
	text_program_header.p_memsz = 0;
	text_program_header.p_flags = PF_R | PF_X;
	text_program_header.p_align = 0;
}

void Assembler::_generate_text(const std::string &p_input_file)
{
	std::ifstream input(p_input_file);
	std::string line;
	while (std::getline(input, line))
	{
		unsigned int column = 0;
		std::string operation = "";
		while (column < line.size())
		{
			column = _get_next_columm(line, column);

			if (line[column] == ':')
			{
				functions.push_back(operation);
				column++;
				continue;
			}

			operation += line[column];

			if (operation == "mov")
			{
				char type = line[++column];
				std::string source = "";
				std::string destination = "";

				column = _get_next_columm(line, ++column);

				if (line[column] == '$')
				{
					column++;
					while (line[column] != ',' && line[column] != ' ' && line[column] != '\t')
					{
						source += line[column];
						column++;
					}
				}

				column = _get_next_columm(line, ++column);
				if (line[column] == '%')
				{
					column++;
					while (line[column] != ',' && line[column] != ' ' && line[column] != '\t' && line[column] != '\0')
					{
						destination += line[column];
						column++;
					}
					text.push_back(_get_mov_register_opcode(destination));
				}

				if (type == 'l')
				{
					_push_int(text, std::stoi(source));
				}
			}

			if (op_opcodes.count(operation) > 0)
			{
				//text.push_back(op_opcodes.at(operation));
				operation = "";
			}

			column++;
		}
	}

	/*
	 * append program exit
	 *
	 *  mov edi,eax
	 *	mov eax,0x3c
	 *	syscall
	 */
	text.push_back(0x89);
	text.push_back(0xC7);

	text.push_back(0xB8);
	text.push_back(0x3C);
	text.push_back(0x00);
	text.push_back(0x00);
	text.push_back(0x00);

	text.push_back(0x0F);
	text.push_back(0x05);
}

char Assembler::_get_mov_register_opcode(const std::string &p_register)
{
	if (mov_register_opcodes.count(p_register) > 0)
	{
		return mov_register_opcodes.at(p_register);
	}

	std::cout << "error: move register '" << p_register << "' not reconized" << std::endl;
	return 0x0;
}

int Assembler::_get_next_columm(const std::string &p_text, int p_column)
{
	while (p_text[p_column] != '\0' && (p_text[p_column] == ' ' || p_text[p_column] == '\t'))
	{
		p_column++;
	}
	return p_column;
}

void Assembler::_push_int(std::vector<unsigned char> &p_vector, int p_value)
{
	p_vector.push_back(p_value & 0xFF);
	p_vector.push_back((p_value >> 8) & 0xFF);
	p_vector.push_back((p_value >> 16) & 0xFF);
	p_vector.push_back((p_value >> 24) & 0xFF);
}

void Assembler::_push_string(std::vector<unsigned char> &p_vector, std::string p_string)
{
	for (char c : p_string)
	{
		p_vector.push_back(c);
	}

	// add null terminator
	p_vector.push_back(0);
}

Assembler::Assembler()
{
}
