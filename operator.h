#pragma once

#include <vector>
#include "token.h"

enum operator_type {
	op_none = -1,
	op_push_string = 0,
	op_push_number = 1,
	op_call_builtin = 2,
	op_call_class = 3,
	op_call_global = 4,
	op_store_global = 5,
	op_load_global = 6,
	op_load_local = 7,
	op_store_local = 8,
	op_add = 9,
	op_sub = 10,
	op_mul = 11,
	op_div = 12,
	op_mod = 13,
	op_pow = 14,
	op_ret = 15,
	op_inc = 16,
	op_dec = 17,
	op_label = 18,
	op_goto = 19,
	op_for = 20,
	op_greater = 21,
	op_lesser = 22,
	op_eq_greater = 23,
	op_eq_lesser = 24,
	op_equal = 25,
	op_not_equal = 26,
	op_if = 27,
	op_new = 28,
	op_load_attr = 29,
	op_store_attr = 30,
	op_store_class = 31,
	op_call_attr = 32,
	op_or = 33,
	op_and = 34,
	op_push_bool = 35,
	op_array = 36,
	op_array_get = 37,
	op_vector = 38,
	op_load_class = 39,
	op_push_null = 40,
	op_push_this = 41,
	op_super_call = 42,
	op_keybaord = 43,
};

class Operator {
private:
	operator_type type;
	int line_number;
public:
	int get_line_number() const;
	operator_type get_type() const;
	std::vector<Token*> operands;
	Operator(operator_type type, std::vector<Token*>& operands, int line_number);
};