#include "cm_array.h"

CMArray::CMArray(unsigned int id, unsigned int parent_id) : CMClass(id, parent_id, 0, 0, 0) {
	this->type = code_member_array;
	this->name = L"array";

	std::vector<Operator*> operators;
	std::vector<std::wstring> params;

	this->member_functions = new std::unordered_map<unsigned int, CMFunction*>;

	constructor = nullptr;
	initializer = nullptr;

	this->member_functions->insert(std::make_pair(0, new CMArrayPush(operators, 0, params)));
	this->member_functions->insert(std::make_pair(1, new CMArraySize(operators, 1, params)));
	this->member_functions->insert(std::make_pair(2, new CMArrayRemove(operators, 2, params)));
	this->member_functions->insert(std::make_pair(3, new CMArraySet(operators, 3, params)));
}

CMArrayPush::CMArrayPush(std::vector<Operator*>& operators, unsigned int id,
	std::vector<std::wstring>& param_types) : CMFunction(L"push", operators, id, L"void", param_types, L"public") {
	this->type = code_member_array_push;
}

CMArrayRemove::CMArrayRemove(std::vector<Operator*>& operators, unsigned int id,
	std::vector<std::wstring>& param_types) : CMFunction(L"remove", operators, id, L"void", param_types, L"public") {
	this->type = code_member_array_remove;
}

CMArraySize::CMArraySize(std::vector<Operator*>& operators, unsigned int id,
	std::vector<std::wstring>& param_types) : CMFunction(L"size", operators, id, L"number", param_types, L"public") {
	this->type = code_member_array_size;
}

CMArraySet::CMArraySet(std::vector<Operator*>& operators, unsigned int id,
	std::vector<std::wstring>& param_types) : CMFunction(L"set", operators, id, L"void", param_types, L"public") {
	this->type = code_member_array_set;
}
