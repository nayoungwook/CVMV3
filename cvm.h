#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <SDL_ttf.h>
#include <stack>

#include "sys_file.h"
#include "parser.h"
#include "code_memory/cm_function.h"
#include "operand.h"
#include "memory.h"
#include "engine/engine.h"
#include "sys_logger.h"
#include "code_memory/cm_shader.h"
#include "gc/gc.h"

static const int SHADER_MEMORY = 0;

static const int BUILTIN_PRINT = 0;
static const int BUILTIN_WINDOW = 1;
static const int BUILTIN_LOAD_SCENE = 2;
static const int BUILTIN_IMAGE = 3;
static const int BUILTIN_BACKGROUND = 4;
static const int BUILTIN_RANDOM = 5;
static const int BUILTIN_SIN = 6;
static const int BUILTIN_COS = 7;
static const int BUILTIN_TAN = 8;
static const int BUILTIN_ATAN = 9;
static const int BUILTIN_ABS = 10;
static const int BUILTIN_RANDOM_RANGE = 11;
static const int BUILTIN_SQRT = 12;
static const int BUILTIN_TEXT = 13;

class FunctionFrame;

class CVM {
public:
	std::queue<std::pair<std::wstring, std::wstring>> load_queue; // <name , path>
	std::unordered_map<std::wstring, CMImage*> resources;

	std::queue<std::pair<std::wstring, std::wstring>> font_queue; // <name , path>
	std::unordered_map<std::wstring, TTF_Font*> font_resources;

	std::vector<FunctionFrame*> stack_area;

	std::unordered_set<std::wstring> imported_files;

	std::unordered_map<unsigned int, CMFunction*> global_functions;
	std::unordered_map<unsigned int, CMClass*> global_class;
	std::unordered_map<unsigned int, CMClass*> builtin_class;

	std::unordered_map<unsigned int, Operand*> global_area;

	std::vector<Memory*> heap_area;

	std::unordered_map<std::wstring, unsigned int>* label_id;

	std::unordered_set<std::wstring> key_data;

	int proj_width, proj_height;

	unsigned int array_code_memory_id = 0;

	CGC* gc;
	static Memory* current_scene_memory;

	SDL_Renderer* renderer;

	CVM();
};

void load_builtin_array(CVM* vm);
void register_parsed_file(std::vector<Token*>& parsed_tokens, CVM* vm);
