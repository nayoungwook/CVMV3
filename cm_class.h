#pragma once

#include <SDL.h>
#include <thread>
#include <gl/glew.h>
#include "code_memory.h"
#include "cm_function.h"
#include <iostream>
#include <unordered_map>

class CMClass : public CodeMemory {
private:
	unsigned int id = 0;
	unsigned int parent_id = 0;

	unsigned int init_function_id = 0;
	unsigned int tick_function_id = 0;
	unsigned int render_function_id = 0;
public:
	std::string name = "";

	std::unordered_map<unsigned int, CMFunction*>* member_functions;

	CMInitialize* initializer = nullptr;
	CMConstructor* constructor = nullptr;

	unsigned int get_id() const;
	unsigned int get_parent_id() const;

	unsigned int get_init_function_id() const;
	unsigned int get_tick_function_id() const;
	unsigned int get_render_function_id() const;

	CMClass(
		unsigned int id, unsigned int parent_id,
		unsigned int init_function_id, unsigned int tick_function_id,
		unsigned int render_function_id
	);
};
