#pragma once

#include <cstdio>
#include <random>
#include <chrono>

#include "code_memory/cm_function.h"
#include "code_memory/cm_window.h"
#include "error/sys_error.h"
#include "sys_util.h"
#include "stack.h"
#include "cvm.h"
#include "memory.h"
#include "gc/gc.h"

class FunctionFrame {
private:
	CMFunction* code_memory;
public:
	std::unordered_map<unsigned int, Operand*> local_area;
	CMFunction* get_code_memory() const;
	void print_operand(Operand* operand);
	FunctionFrame(CMFunction* code_memory);
	~FunctionFrame();
	Stack* stack;
	void run(CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void run_builtin(code_type cm_type, CVM* vm, FunctionFrame* caller, Memory* caller_class);

	void object_builtin_render(CVM* vm, FunctionFrame* caller, Memory* caller_class);

	void run_builtin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);


	void builtin_image(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_print(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_window(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_load_scene(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_background(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_random(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_sin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_cos(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_tan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_atan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_abs(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_random_range(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_sqrt(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_text(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
	void builtin_color(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class);
};

void add_parameter(bool is_defined_function, FunctionFrame* frame, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count);
void run_function(CVM* vm, Memory* caller_class, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count);
Operand* calcaulte_vector_operand(Operand* lhs, Operand* rhs, float (*cal)(float l, float r));
Memory* create_object(CVM* vm, std::unordered_map<unsigned int, CMClass*>::iterator code_memory, FunctionFrame* frame, unsigned int constructor_parameter_count);

Operand* cast_operand(Operator* op, std::wstring cast_type, Operand* target);

const inline std::wstring get_type_string_of_operand(Operand* op);
void check_type_for_store(CVM* vm, std::wstring const& type1, std::wstring const& type2);

float cal_add(float lhs, float rhs);
float cal_sub(float lhs, float rhs);
float cal_mult(float lhs, float rhs);
float cal_div(float lhs, float rhs);
float cal_mod(float lhs, float rhs);
float cal_pow(float lhs, float rhs);

bool operand_compare(Operand* op1, Operand* op2);