#pragma once

#include "../operator.h"
#include "code_memory.h"
#include <vector>

class CMFunction : public CodeMemory {
private:
	std::vector<Operator*> operators;
	std::vector<std::string> param_types;
	unsigned int id;
	std::string return_type = "";
	std::string access_modifier = "";
public:
	std::string name = "";
	std::string get_return_type() const;
	std::string get_access_modifier() const;
	std::vector<Operator*> get_operators() const;
	std::vector<std::string> get_param_types() const;

	unsigned int get_id() const;
	CMFunction(std::string const& name, std::vector<Operator*>& operators, unsigned int& id, std::string const& return_type, std::vector<std::string>& param_types, std::string const& access_modifier);
};

class CMInitialize : public CMFunction {
public:
	CMInitialize(std::vector<Operator*>& operators, unsigned int& id, std::string const& return_type,
		std::vector<std::string>& param_types, std::string const& access_modifier);
};

class CMConstructor : public CMFunction {
public:
	CMConstructor(std::vector<Operator*>& operators, unsigned int& id, std::string const& return_type,
		std::vector<std::string>& param_types, std::string const& access_modifier);
};

class CMRender : public CMFunction {
public:
	CMRender(std::vector<Operator*>& operators, unsigned int id, std::vector<std::string>& param_types);
};
