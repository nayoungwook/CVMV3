#include "cm_class.h"

CMClass::CMClass(unsigned int id, unsigned int parent_id,
	unsigned int init_function_id, unsigned int tick_function_id,
	unsigned int render_function_id)
	: id(id), parent_id(parent_id), init_function_id(init_function_id),
	tick_function_id(tick_function_id), render_function_id(render_function_id) {
	this->type = code_class;
}

unsigned int CMClass::get_id() const {
	return this->id;
}

unsigned int CMClass::get_parent_id() const {
	return this->parent_id;
}

unsigned int CMClass::get_init_function_id() const {
	return this->init_function_id;
}

unsigned int CMClass::get_tick_function_id() const {
	return this->tick_function_id;
}

unsigned int CMClass::get_render_function_id() const {
	return this->render_function_id;
}
