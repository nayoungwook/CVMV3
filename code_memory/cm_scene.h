#pragma once

#include "../memory.h"
#include "../operator.h"
#include "code_memory.h"
#include "cm_function.h"

class CMScene : public CMClass {
private:
public:
	CMScene(unsigned int id, unsigned int parent_id,
		unsigned int init_function_id, unsigned int tick_function_id,
		unsigned int render_function_id);
};
