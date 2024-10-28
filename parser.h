#pragma once

#include "memory.h"
#include "code_memory/code_memory.h"
#include <vector>
#include "token.h"
#include "operator.h"
#include "code_memory/cm_function.h"
#include "code_memory/cm_scene.h"
#include "code_memory/cm_object.h"
#include "code_memory/cm_image.h"
#include "sys_file.h"
#include <assert.h>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "cvm.h"

class CVM;

Operator* get_operator(CVM* vm, std::unordered_map<std::wstring, unsigned int>* label_id, int line_number, std::vector<Token*>& tokens);
CodeMemory* get_code_memory(CVM* vm, std::vector<Token*>& tokens);

enum class special_literal {
	line_break, none,
};
