#pragma once

enum code_type {
	code_function = 0,
	code_class = 1,
	code_initialize = 2,
	code_constructor = 3,
	code_scene = 4,
	code_object = 5,

	code_member_render = 6,
	code_member_array = 7,
	code_member_array_push = 8,
	code_member_array_remove = 9,
	code_member_array_size = 10,
	code_member_array_set = 11,
	code_member_array_render = 12,
	code_member_array_tick = 13,
};

class CodeMemory {
protected:
	code_type type;

public:
	code_type get_type() const;
};