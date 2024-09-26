#pragma once

#include "../operator.h"
#include "code_memory.h"
#include <vector>

class CMFunction : public CodeMemory {
private:
	std::vector<Operator*> operators;
	std::vector<std::wstring> param_types;
	unsigned int id;
	std::wstring return_type = L"";
	std::wstring access_modifier = L"";
public:
	std::wstring name = L"";
	std::wstring get_return_type() const;
	std::wstring get_access_modifier() const;
	std::vector<Operator*> get_operators() const;
	std::vector<std::wstring> get_param_types() const;

	unsigned int get_id() const;
	CMFunction(std::wstring const& name, std::vector<Operator*>& operators, unsigned int& id, std::wstring const& return_type, std::vector<std::wstring>& param_types, std::wstring const& access_modifier);
};

class CMInitialize : public CMFunction {
public:
	CMInitialize(std::vector<Operator*>& operators, unsigned int& id, std::wstring const& return_type,
		std::vector<std::wstring>& param_types, std::wstring const& access_modifier);
};

class CMConstructor : public CMFunction {
public:
	CMConstructor(std::vector<Operator*>& operators, unsigned int& id, std::wstring const& return_type,
		std::vector<std::wstring>& param_types, std::wstring const& access_modifier);
};

class CMRender : public CMFunction {
public:
	CMRender(std::vector<Operator*>& operators, unsigned int id, std::vector<std::wstring>& param_types);
};
