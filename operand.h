#pragma once

#include <iostream>
#include <vector>
#include <string>

enum operand_type {
	operand_number = 0,
	operand_string = 1,
	operand_bool = 2,
	operand_address = 3,
	operand_vector = 4,
	operand_op_address = 5,
	operand_null = 6,
};

class Memory;
class Operand {
private:
	std::wstring data;
	operand_type type;
	std::vector<Operand*>* vector_elements;
public:
	void set_data(std::wstring data);
	std::wstring get_data() const;
	std::vector<Operand*>* get_vector_elements();
	double num_data = 0;

	operand_type get_type() const;
	Operand(std::wstring const& data, operand_type type);
	Operand(std::vector<Operand*>* array_data, operand_type type);
	~Operand();

	std::wstring variable_name = L"";
};

Operand* create_address_operand(Memory* op);
Operand* create_op_address_operand(Operand* op);
Operand* copy_operand(Operand* op);
Operand* extract_value_of_opernad(Operand* op);