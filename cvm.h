#pragma once

#include <iostream>
#include <unordered_map>

#include "sys_file.h"
#include "parser.h"
#include "cm_function.h"
#include "operand.h"
#include "memory.h"

class CVM {
public:

	std::unordered_map<unsigned int, CMFunction*> global_functions;
	std::unordered_map<unsigned int, CMClass*> global_class;

	std::unordered_map<unsigned int, Operand*> global_area;

	std::vector<Memory*> heap_area;

	std::unordered_map<std::string, unsigned int>* label_id;

	static Memory* current_scene_memory;
};
