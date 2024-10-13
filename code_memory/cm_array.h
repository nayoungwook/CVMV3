#pragma once

#include "../memory.h"
#include "../operator.h"
#include "code_memory.h"
#include "cm_function.h"

class CMArray : public CMClass {
private:
public:
	CMArray(unsigned int id, unsigned int parent_id);
};

class CMArrayPush : public CMFunction {
public:
	CMArrayPush(std::vector<Operator*>& operators, unsigned int id,
		std::vector<std::wstring>& param_types);
};

class CMArrayRemove : public CMFunction {
public:
	CMArrayRemove(std::vector<Operator*>& operators, unsigned int id,
		std::vector<std::wstring>& param_types);
};

class CMArraySize : public CMFunction {
public:
	CMArraySize(std::vector<Operator*>& operators, unsigned int id,
		std::vector<std::wstring>& param_types);
};

class CMArraySet: public CMFunction {
public:
	CMArraySet(std::vector<Operator*>& operators, unsigned int id,
		std::vector<std::wstring>& param_types);
};
