#include "cm_object.h"

CMObject::CMObject(unsigned int id, unsigned int parent_id,
	unsigned int init_function_id, unsigned int tick_function_id,
	unsigned int render_function_id)
	: CMClass(id, parent_id, init_function_id, tick_function_id, render_function_id) {
	this->type = code_object;
}
