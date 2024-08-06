#include "memory.h"

CMClass* Memory::get_cm_class() {
	return cm_class;
}

void Memory::set_cm_class(CMClass* cm_class) {
	this->cm_class = cm_class;
}

Memory::Memory(CMClass* cm_class) : cm_class(cm_class) {

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
