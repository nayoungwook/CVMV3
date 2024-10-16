#include "memory.h"

CMClass* Memory::get_cm_class() {
	return cm_class->second;
}

CMClass* Memory::get_backup_cm_class() {
	return backup_class;
}

void Memory::update_backup_cm_class() {
	backup_class = this->cm_class->second;
}

void Memory::set_cm_class(CMClass* cm_class) {
	this->cm_class->second = cm_class;
}

ArrayMemory::ArrayMemory(std::unordered_map<unsigned int, CMClass*>::iterator cm_class, std::vector<Operand*>* array_elements)
	: Memory(cm_class), array_elements(array_elements) {
	this->type = L"array";
}

Memory::Memory(std::unordered_map<unsigned int, CMClass*>::iterator cm_class) : cm_class(cm_class) {

}

ArrayMemory::~ArrayMemory() {
	for (Operand* op : *array_elements) {
		delete op;
	}

	delete array_elements;
}

Memory::~Memory() {
	std::unordered_map<unsigned int, Operand*>::iterator member_iter;
	for (member_iter = this->member_variables.begin(); member_iter != this->member_variables.end(); member_iter++) {
		delete member_iter->second;
	}

	if (this->get_cm_class()->get_type() == code_object) {
	}

	if (this->parent_memory != nullptr) delete this->parent_memory;
}

void Memory::store_parent_memory(Memory* memory) {
	if (memory != nullptr) {
		std::cout << "Error at memory\.cpp , parent already exist." << std::endl;
	}

	this->parent_memory = memory;
}

Memory* Memory::get_parent_memory() {
	return this->parent_memory;
}
