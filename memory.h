#pragma once

#include "operand.h"
#include "code_memory/cm_class.h"

class Memory {
private:
	CMClass* cm_class = nullptr;
	Memory* parent_memory = nullptr;
public:
	std::unordered_map<unsigned int, Operand*> member_variables;
	void store_parent_memory(Memory* memory);
	Memory* get_parent_memory();
	CMClass* get_cm_class();
	void set_cm_class(CMClass* cm_class);
	Memory(CMClass* cm_class);
};
