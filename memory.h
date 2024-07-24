#pragma once

#include "operand.h"
#include "code_memory/cm_class.h"

class Memory {
private:
	CMClass* cm_class;
public:
	std::unordered_map<unsigned int, Operand*> member_variables;
	CMClass* get_cm_class();
	Memory(CMClass* cm_class);
};
