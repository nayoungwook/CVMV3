#pragma once

#include <iostream>
#include <vector>

enum operand_type {
	operand_number = 0,
	operand_string = 1,
	operand_bool = 2,
	operand_address = 3,
	operand_array = 4,
	operand_vector = 5,
	operand_op_address = 6,
};

class Operand {
private:
	std::string data;
	operand_type type;
	std::vector<Operand*> array_data;
public:
	void set_data(std::string data);
	std::string get_data() const;
	std::vector<Operand*> get_array_data() const;

	operand_type get_type() const;
	Operand(std::string const& data, operand_type type);
	Operand(std::vector<Operand*>& array_data, operand_type type);
};
