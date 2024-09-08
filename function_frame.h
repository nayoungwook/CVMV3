#pragma once

#include <cstdio>
#include <random>
#include "code_memory/cm_function.h"
#include "code_memory/cm_window.h"
#include "error/sys_error.h"
#include "sys_util.h"
#include "stack.h"
#include "cvm.h"
#include "memory.h"

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
};

Operand* create_address_operand(Memory* op);
Operand* create_op_address_operand(Operand* op);
Operand* copy_operand(Operand* op);
inline Operand* extract_value_of_opernad(Operand* op);
void run_function(CVM* vm, Memory* caller_class, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count);
Operand* calcaulte_vector_operand(Operand* lhs, Operand* rhs, double (*cal)(double l, double r));
Memory* create_object(CVM* vm, std::unordered_map<unsigned int, CMClass*>::iterator code_memory, FunctionFrame* frame, unsigned int constructor_parameter_count);

const std::string get_type_string_of_operand(Operand* op);
void check_type_for_store(CVM* vm, std::string const& type1, std::string const& type2);

double cal_add(double lhs, double rhs);
double cal_sub(double lhs, double rhs);
double cal_mult(double lhs, double rhs);
double cal_div(double lhs, double rhs);
double cal_mod(double lhs, double rhs);
double cal_pow(double lhs, double rhs);

bool operand_compare(Operand* op1, Operand* op2);