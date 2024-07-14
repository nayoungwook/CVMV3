#include "operand.h"

Operand::Operand(std::string const& data, operand_type type) : data(data), type(type) {

}

Operand::Operand(std::vector<Operand*>& array_data) : array_data(array_data), type(operand_array) {

}

std::vector<Operand*> Operand::get_array_data() const {
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
