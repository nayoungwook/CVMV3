#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cwchar>

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
	operand_type type;
	std::vector<Operand*>* vector_elements;
public:
	std::vector<Operand*>* get_vector_elements();

	void* data;
	int size;

	operand_type get_type() const;
	Operand(int size, operand_type type);
	Operand(std::vector<Operand*>* array_data, operand_type type);
	~Operand();

	template <typename T>
	T get_number_data();

	std::wstring variable_name = L"";
};

template<typename T>
T Operand::get_number_data() {
	switch (type)
	{
	case operand_number:
		return (T)* (double*)data;
	}
}

Operand* create_address_operand(Memory* op);
Operand* create_op_address_operand(Operand* op);
Operand* copy_operand(Operand* op);
Operand* extract_value_of_opernad(Operand* op);