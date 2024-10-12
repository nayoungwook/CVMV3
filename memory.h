#pragma once

#include "operand.h"
#include "code_memory/cm_class.h"

class Memory {
private:
	std::unordered_map<unsigned int, CMClass*>::iterator cm_class;
	Memory* parent_memory = nullptr;
	CMClass* backup_class = nullptr;
public:
	std::unordered_map<unsigned int, Operand*> member_variables;
	std::unordered_map<unsigned int, std::wstring> member_variable_names;
	void store_parent_memory(Memory* memory);
	Memory* get_parent_memory();
	void update_backup_cm_class();
	CMClass* get_cm_class();
	CMClass* get_backup_cm_class();
	void set_cm_class(CMClass* cm_class);
	Memory(std::unordered_map<unsigned int, CMClass*>::iterator cm_class);
	~Memory();
};
