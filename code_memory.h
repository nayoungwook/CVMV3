#pragma once

enum code_type {
	code_function = 0,
	code_class = 1,
	code_initialize = 2,
	code_constructor = 3,
	code_scene = 4,
	code_object = 5,
};

class CodeMemory {
protected:
	code_type type;

public:
	code_type get_type() const;
};