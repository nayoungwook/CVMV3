#pragma once

#include "../memory.h"
#include "../operator.h"
#include "code_memory.h"
#include "cm_function.h"

static const int OBJECT_POSITION = 0;
static const int OBJECT_WIDTH = 1;
static const int OBJECT_HEIGHT = 2;
static const int OBJECT_ROTATION = 3;
static const int OBJECT_SPRITE = 4;

class CMObject : public CMClass {
private:
public:
	CMObject(unsigned int id, unsigned int parent_id,
		unsigned int init_function_id, unsigned int tick_function_id,
		unsigned int render_function_id);
};
