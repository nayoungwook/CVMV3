#include "cm_function.h"

CMFunction::CMFunction(std::vector<Operator*>& operators, unsigned int& id, std::string const& return_type,
	std::vector<std::string>& param_types, std::string const& access_modifier)
	: operators(operators), id(id), return_type(return_type), param_types(param_types), access_modifier(access_modifier) {
	this->type = code_function;
}

CMInitialize::CMInitialize(std::vector<Operator*>& operators, unsigned int& id, std::string const& return_type,
	std::vector<std::string>& param_types, std::string const& access_modifier)
	: CMFunction(operators, id, return_type, param_types, access_modifier) {
	this->type = code_initialize;
}

CMConstructor::CMConstructor(std::vector<Operator*>& operators, unsigned int& id, std::string const& return_type,
	std::vector<std::string>& param_types, std::string const& access_modifier)
	: CMFunction(operators, id, return_type, param_types, access_modifier) {
	this->type = code_constructor;
}

unsigned int CMFunction::get_id() const {
	return this->id;
}

std::string CMFunction::get_return_type() const {
	return this->return_type;
}

std::string CMFunction::get_access_modifier() const {
	return this->access_modifier;
}

std::vector<std::string> CMFunction::get_param_types() const {
	return this->param_types;
}

std::vector<Operator*> CMFunction::get_operators() const {
	return this->operators;
}