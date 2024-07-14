#pragma once

#include <iostream>

enum operand_type {
	operand_number = 0,
	operand_string = 1,
	operand_bool = 2,
	operand_address = 3,

};

class Operand {
private:
	std::string data;
	operand_type type;
public:
	void set_data(std::string data);
	std::string get_data() const;
	operand_type get_type() const;
	Operand(std::string const& data, operand_type type);
};
