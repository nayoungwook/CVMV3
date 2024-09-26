#include "cm_function.h"

CMFunction::CMFunction(std::wstring const& name, std::vector<Operator*>& operators,
	unsigned int& id, std::wstring const& return_type,
	std::vector<std::wstring>& param_types, std::wstring const& access_modifier)
	: name(name), operators(operators), id(id), return_type(return_type), param_types(param_types), access_modifier(access_modifier) {
	this->type = code_function;
}

CMInitialize::CMInitialize(std::vector<Operator*>& operators, unsigned int& id, std::wstring const& return_type,
	std::vector<std::wstring>& param_types, std::wstring const& access_modifier)
	: CMFunction(L"initialize", operators, id, return_type, param_types, access_modifier) {
	this->type = code_initialize;
}

CMConstructor::CMConstructor(std::vector<Operator*>& operators, unsigned int& id, std::wstring const& return_type,
	std::vector<std::wstring>& param_types, std::wstring const& access_modifier)
	: CMFunction(L"constructor", operators, id, return_type, param_types, access_modifier) {
	this->type = code_constructor;
}

CMRender::CMRender(std::vector<Operator*>& operators, unsigned int id, std::vector<std::wstring>& param_types)
	: CMFunction(L"render", operators, id, L"void", param_types, L"public") {
	this->type = code_render;
}

unsigned int CMFunction::get_id() const {
	return this->id;
}

std::wstring CMFunction::get_return_type() const {
	return this->return_type;
}

std::wstring CMFunction::get_access_modifier() const {
	return this->access_modifier;
}

std::vector<std::wstring> CMFunction::get_param_types() const {
	return this->param_types;
}

std::vector<Operator*> CMFunction::get_operators() const {
	return this->operators;
}