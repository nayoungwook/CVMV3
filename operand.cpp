#include "operand.h"

Operand::Operand(std::wstring const& data, operand_type type) : data(data), type(type) {
}

Operand::~Operand() {
	if (this->type == operand_vector) {
		for (int i = 0; i < vector_elements->size(); i++) {
			delete vector_elements->at(i);
		}
		delete vector_elements;
	}
}

Operand::Operand(std::vector<Operand*>* array_data, operand_type type)
	: type(type) {
	this->vector_elements = array_data;
}

std::vector<Operand*>* Operand::get_vector_elements() {
	return vector_elements;
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

Operand* extract_value_of_opernad(Operand* op) {

	Operand* result = op;

	if (op->get_type() == operand_op_address) {
		while (result->get_type() == operand_op_address) {
			result = reinterpret_cast<Operand*>(std::stoull(result->get_data()));
		}
	}

	return result;
}

Operand* copy_operand(Operand* op) {

	Operand* copied_op = nullptr;

	op = extract_value_of_opernad(op);

	switch (op->get_type()) {
	case operand_vector: {
		std::vector<Operand*>* array_data = new std::vector<Operand*>;
		std::vector<Operand*>* old_array_data = op->get_vector_elements();

		for (Operand* op : *old_array_data) {
			array_data->push_back(copy_operand(op));
		}

		copied_op = new Operand(array_data, op->get_type());
		break;
	}
	default: {
		copied_op = new Operand(op->get_data(), op->get_type());
		break;
	}
	}

	return copied_op;
}

Operand* create_address_operand(Memory* op) {
	return new Operand(std::to_wstring((unsigned long long)(void**) op), operand_address);
}

Operand* create_op_address_operand(Operand* op) {
	return new Operand(std::to_wstring((unsigned long long)(void**) op), operand_op_address);
}