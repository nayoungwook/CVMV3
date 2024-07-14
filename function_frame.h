#pragma once

#include <cstdio>
#include "cm_function.h"
#include "cm_window.h"
#include "stack.h"
#include "cvm.h"
#include "memory.h"

class FunctionFrame {
private:
	CMFunction* code_memory;
public:
	std::unordered_map<unsigned int, Operand*> local_area;
	CMFunction* get_code_memory() const;
	FunctionFrame(CMFunction* code_memory);
	Stack* stack;
	void run(CVM* vm, FunctionFrame* caller, Memory* caller_class);
};

void run_function(CVM* vm, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count);