#include "operand.h"

Operand::Operand(std::string const& data, operand_type type) : data(data), type(type) {

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
