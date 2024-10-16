#include "operand.h"

Operand::Operand(int size, operand_type type) : type(type), size(size) {
	if (size <= 0)
		data = nullptr;
	else {
		if (type == operand_string)
			data = (void*)malloc(sizeof(wchar_t) * (size + 1));
		else
			data = (void*)malloc(sizeof(char) * size);
	}
}

Operand::~Operand() {
	if (this->type == operand_vector) {
		for (int i = 0; i < vector_elements->size(); i++) {
			delete vector_elements->at(i);
		}
		delete vector_elements;
	}

	if (type == operand_number) {
		double d = *((double*)data);
	}

	if (this->type != operand_address && this->type != operand_op_address && data != nullptr && this->type != operand_string)
		delete data;
}

Operand::Operand(std::vector<Operand*>* array_data, operand_type type)
	: type(type) {
	this->vector_elements = array_data;
}

std::vector<Operand*>* Operand::get_vector_elements() {
	return vector_elements;
}

operand_type Operand::get_type() const {
	return type;
}

Operand* extract_value_of_opernad(Operand* op) {

	Operand* result = op;

	if (op->get_type() == operand_op_address) {
		while (result->get_type() == operand_op_address) {
			result = (Operand*)result->data;
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

	case operand_address:
	case operand_op_address:
	{
		copied_op = new Operand(0, op->get_type());
		copied_op->data = op->data;
		break;
	}

	default: {
		copied_op = new Operand(op->size, op->get_type());
		
		if (op->get_type() == operand_string) {
			copied_op->data = op->data;
		}
		else {
			memcpy(copied_op->data, op->data, sizeof(char) * op->size);
		}

		break;
	}
	}

	return copied_op;
}

Operand* create_address_operand(Memory* op) {
	Operand* result = new Operand(0, operand_address);
	result->data = op;
	return result;
}

Operand* create_op_address_operand(Operand* op) {
	Operand* result = new Operand(0, operand_op_address);
	result->data = op;
	return result;
}