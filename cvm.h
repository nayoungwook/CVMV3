#pragma once

#include <iostream>
#include <unordered_map>
#include <queue>

#include "sys_file.h"
#include "parser.h"
#include "cm_function.h"
#include "operand.h"
#include "memory.h"
#include "engine.h"
#include "sys_logger.h"
#include "cm_shader.h"

static const int SHADER_MEMORY = 0;

static const int BUILTIN_PRINT = 0;
static const int BUILTIN_WINDOW = 1;
static const int BUILTIN_LOAD_SCENE = 2;
static const int BUILTIN_IMAGE = 3;

static const int OBJECT_POSITION = 0;
static const int OBJECT_WIDTH = 1;
static const int OBJECT_HEIGHT = 2;
static const int OBJECT_ROTATION = 3;
static const int OBJECT_SPRITE = 4;

class CVM {
public:
	std::queue<std::pair<std::string, std::string>> load_queue; // <name , path>
	std::unordered_map<std::string, CMImage*> resources;

	std::unordered_map<unsigned int, CMFunction*> global_functions;
	std::unordered_map<unsigned int, CMClass*> global_class;

	std::unordered_map<unsigned int, Operand*> global_area;

	std::vector<Memory*> heap_area;

	std::unordered_map<std::string, unsigned int>* label_id;

	static Memory* current_scene_memory;
};
