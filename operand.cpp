#include "operand.h"

Operand::Operand(std::wstring const& data, operand_type type) : data(data), type(type) {
}

Operand::~Operand() {
	if (this->type == operand_vector) {
		for (int i = 0; i < array_data->size(); i++) {
			delete array_data->at(i);
		}
		delete array_data;
	}
}

Operand::Operand(std::vector<Operand*>* array_data, operand_type type)
	: type(type) {
	this->array_data = array_data;
}

std::vector<Operand*>* Operand::get_array_data() {
	return array_data;
}

void Operand::set_data(std::wstring data) {
	this->data = data;
}

std::wstring Operand::get_data() const {
	return data;
}

operand_type Operand::get_type() const {
	return type;
}
