#pragma once

#include "memory.h"
#include "code_memory.h"
#include <vector>
#include "token.h"
#include "operator.h"
#include "cm_function.h"
#include "cm_scene.h"
#include "cm_image.h"
#include "sys_file.h"
#include <assert.h>
#include <queue>
#include <unordered_map>

Operator* get_operator(std::unordered_map<std::string, unsigned int>* label_id, int line_number, std::vector<Token*>& tokens);
CodeMemory* get_code_memory(std::queue<std::pair<std::string, std::string>>& load_queue, std::unordered_map<std::string, unsigned int>* label_id, std::vector<Token*>& tokens);

enum class special_literal {
	line_break, none,
};
