#pragma once

#include "memory.h"
#include "operator.h"
#include "code_memory.h"
#include "cm_function.h"

static const int OBJECT_RENDER_FUNCTION = 0;

class CMObject : public CMClass {
private:
public:
	CMObject(unsigned int id, unsigned int parent_id,
		unsigned int init_function_id, unsigned int tick_function_id,
		unsigned int render_function_id);
};
