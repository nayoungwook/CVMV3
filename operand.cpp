#include "operand.h"

Operand::Operand(int i) {
	this->type = operand_integer;
	data = (int*)malloc(sizeof(int));
	*(int*)data = i;
}

Operand::Operand(float f) {
	this->type = operand_float;
	data = (float*)malloc(sizeof(float));
	*(float*)data = f;
}

Operand::Operand(double d) {
	this->type = operand_number;
	data = (double*)malloc(sizeof(double));
	*(double*)data = d;
}

Operand::Operand(bool b) {
	this->type = operand_bool;
	data = (bool*)malloc(sizeof(bool));
	*(bool*)data = b;
}

Operand::Operand(std::wstring str) {
	this->type = operand_string;
	data = new std::wstring();
	*(std::wstring*)data = str;
}

Operand::Operand(Memory* add) {
	this->type = operand_address;
	this->data = add;
}

Operand::Operand() {
	this->type = operand_null;
}

void Operand::increase() {
	switch (type) {
	case operand_number:
		*((double*)data) = *((double*)data) + 1;
		break;
	case operand_float:
		*((float*)data) = *((float*)data) + 1;
		break;
	case operand_integer:
		*((int*)data) = *((int*)data) + 1;
		break;
	}
}

void Operand::decrease() {
	switch (type) {
	case operand_number:
		*((double*)data) = *((double*)data) - 1;
		break;
	case operand_float:
		*((float*)data) = *((float*)data) - 1;
		break;
	case operand_integer:
		*((int*)data) = *((int*)data) - 1;
		break;
	}
}

Memory* Operand::get_memory_data() {
	switch (type) {
	case operand_address:
		return (Memory*)data;
	}
}

bool Operand::get_bool_data() {
	switch (type) {
	case operand_bool:
		return (bool)*(bool*)data;
	}
}

Operand::~Operand() {
	if (this->type == operand_vector) {
		for (int i = 0; i < vector_elements->size(); i++) {
			delete vector_elements->at(i);
		}
		delete vector_elements;
	}

	if (this->type != operand_address && data != nullptr && this->type != operand_string)
		delete data;
}

Operand::Operand(std::vector<Operand*>* array_data, operand_type type)
	: type(type) {
	this->vector_elements = array_data;
}

std::vector<Operand*>* Operand::get_vector_elements() {
	return vector_elements;
}

bool Operand::is_number_type() {
	return type == operand_number || type == operand_integer || type == operand_float;
}

operand_type Operand::get_type() const {
	return type;
}

Operand* copy_operand(Operand* op) {

	Operand* copied_op = nullptr;

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

	case operand_number: {
		copied_op = new Operand(op->get_number_data<double>());
		break;
	}

	case operand_float: {
		copied_op = new Operand(op->get_number_data<float>());
		break;
	}

	case operand_integer: {
		copied_op = new Operand(op->get_number_data<int>());
		break;
	}

	case operand_bool: {
		copied_op = new Operand(op->get_bool_data());
		break;
	}

	case operand_string: {
		copied_op = new Operand(op->get_string_data<std::wstring>());
		break;
	}

	case operand_address:
	{
		copied_op = new Operand((Memory*)op->data);
		break;
	}
	case operand_null:
	{
		copied_op = new Operand();
		break;
	}
	}

	return copied_op;
}

Operand* create_address_operand(Memory* op) {
	Operand* result = new Operand(op);
	return result;
}
