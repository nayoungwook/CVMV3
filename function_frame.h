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
	~FunctionFrame();
	Stack* stack;
	void run(CVM* vm, FunctionFrame* caller, Memory* caller_class);

	void run_builtin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);


	void builtin_image(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_print(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_window(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_load_scene(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
};

Operand* create_address_operand(Memory* op);
Operand* create_op_address_operand(Operand* op);
Operand* copy_operand(Operand* op);
inline Operand* extract_value_of_opernad(Operand* op);
void run_function(CVM* vm, Memory* caller_class, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count);