#include "operand.h"

Operand::Operand(std::string const& data, operand_type type) : data(data), type(type) {
	std::vector<Operand*> _v;

	if (type == operand_number) {
		if ((int)std::stod(data) == std::stod(data)) {
			this->data = std::to_string((int)std::stod(data));
		}
	}
}

Operand::~Operand() {
}

Operand::Operand(std::vector<Operand*>& array_data, operand_type type) : type(type) {
	this->array_data = array_data;
}

std::vector<Operand*>& Operand::get_array_data() {
	return array_data;
}

void Operand::set_data(std::string data) {
	this->data = data;
}

std::string Operand::get_data() const {
	return data;
}

operand_type Operand::get_type() const {
	return type;
}
