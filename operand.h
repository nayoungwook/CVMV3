#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cwchar>

enum operand_type {
	operand_none = -1,
	operand_number = 0,
	operand_string = 1,
	operand_bool = 2,
	operand_address = 3,
	operand_vector = 4,
	operand_null = 5,
	operand_integer = 6,
	operand_float = 7,
};

class Memory;
class Operand {
private:
	operand_type type = operand_none;
	std::vector<Operand*>* vector_elements;
public:
	std::vector<Operand*>* get_vector_elements();

	void* data = nullptr;
	int size;

	void increase();
	void decrease();

	bool is_number_type();

	operand_type get_type() const;

	Operand(int i);
	Operand(float f);
	Operand(double d);
	Operand(bool b);
	Operand(std::wstring str);
	Operand(Memory* add);
	Operand(Operand* add);
	Operand();

	Operand(std::vector<Operand*>* array_data, operand_type type);
	~Operand();

	template <typename T>
	T get_number_data();

	template <typename T>
	T get_string_data();

	bool get_bool_data();

	Memory* get_memory_data();

	std::wstring variable_name = L"";
};

template<typename T>
T Operand::get_number_data() {
	switch (type)
	{
	case operand_number:
		return (T) * (double*)data;
	case operand_float:
		return (T) * (float*)data;
	case operand_integer:
		return (T) * (int*)data;
	}
}

template<typename T>
T Operand::get_string_data() {
	switch (type) {
	case operand_string:
		return (T) * (std::wstring*)data;
	}
}

Operand* create_address_operand(Memory* op);
Operand* copy_operand(Operand* op);
