#include "function_frame.h"

extern std::unordered_map<Memory*, Node*> gc_nodes;

CMFunction* FunctionFrame::get_code_memory() const {
	return code_memory;
}

FunctionFrame::FunctionFrame(CMFunction* code_memory) : code_memory(code_memory) {
	this->stack = new Stack();
}

FunctionFrame::~FunctionFrame() {
	std::unordered_map<unsigned int, Operand*>::iterator local_iter;

	for (local_iter = this->local_area.begin(); local_iter != this->local_area.end(); local_iter++) {
		delete local_iter->second;
	}
	delete stack;
}

const inline std::wstring get_type_name_of_operand(Operand* op) {
	std::wstring type_string_operand = L"";

	Operand* extracted_op = op;

	switch (extracted_op->get_type()) {
	case operand_number:
		type_string_operand = L"number";
		break;
	case operand_float:
		type_string_operand = L"float";
		break;
	case operand_integer:
		type_string_operand = L"int";
		break;
	case operand_none:
		type_string_operand = L"none";
		break;
	case operand_null:
		type_string_operand = L"null";
		break;
	case operand_address:
		if (((Memory*)op->data)->type == L"memory")
			type_string_operand = ((Memory*)op->data)->get_cm_class()->name;
		else
			type_string_operand = L"array";
		break;
	case operand_bool:
		type_string_operand = L"bool";
		break;
	case operand_string:
		type_string_operand = L"string";
		break;
	case operand_vector:
		type_string_operand = L"vector";
		break;
	}

	return type_string_operand;
}

void add_parameter(bool is_defined_function, FunctionFrame* frame, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count) {
	if (is_defined_function) {
		for (int i = 0; i < parameter_count; i++) {
			Operand* op = caller_frame->stack->peek();
			caller_frame->stack->pop();

			// TODO : do some type checking for parameters.
			/*

				std::wstring type_string_of_operand = get_type_string_of_operand(op);

				bool wrong_type = false;

				if (code_memory->get_param_types()[i] != type_string_of_operand) {
					wrong_type = true;
				}

				if (wrong_type) {
					std::wstring name = caller_frame->get_code_memory()->name;
					CHESTNUT_THROW_ERROR(L"Failed to call " + std::wstring(name.begin(), name.end()) + L". You pass the wrong parameters",
						"RUNTIME_WRONG_PARAMETER", "0x02", 0);
				}
			*/

			frame->local_area.insert(std::make_pair(i, copy_operand(op)));
		}
	}
	else {
		for (int i = 0; i < parameter_count; i++) {
			frame->stack->push(copy_operand(caller_frame->stack->peek()));
			caller_frame->stack->pop();
		}
	}
}

void run_function(CVM* vm, Memory* caller_class, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count) {
	FunctionFrame* frame = new FunctionFrame(code_memory);

	bool is_defined_function
		= code_memory->get_type() == code_function || code_memory->get_type() == code_constructor || code_memory->get_type() == code_initialize;

	add_parameter(is_defined_function, frame, caller_frame, code_memory, parameter_count);

	frame->run(vm, caller_frame, caller_class);
}

Operand* calcaulte_vector_operand(Operand* lhs, Operand* rhs, float (*cal)(float l, float r)) {
	Operand* lhs_vector = lhs;
	Operand* rhs_vector = rhs;

	std::vector<Operand*>* calculated_result = new std::vector<Operand*>;

	size_t min_vector_size = (size_t)min(lhs_vector->get_vector_elements()->size(), rhs_vector->get_vector_elements()->size());
	for (int i = 0; i < min_vector_size; i++) {
		float lhs_v = lhs_vector->get_vector_elements()->at(i)->get_number_data<float>();
		float rhs_v = rhs_vector->get_vector_elements()->at(i)->get_number_data<float>();

		float calculated_v = cal(lhs_v, rhs_v);
		Operand* op = new Operand(calculated_v);

		calculated_result->push_back(op);
	}

	return new Operand(calculated_result, operand_vector);
}

float cal_add(float lhs, float rhs) {
	return lhs + rhs;
}

float cal_sub(float lhs, float rhs) {
	return lhs - rhs;
}

float cal_mult(float lhs, float rhs) {
	return lhs * rhs;
}

float cal_div(float lhs, float rhs) {
	if (rhs != 0) {
		return lhs / rhs;
	}
	return 0;
}

float cal_mod(float lhs, float rhs) {
	if (rhs != 0) {
		return (int)lhs % (int)rhs;
	}
	return 0;
}

float cal_pow(float lhs, float rhs) {
	return pow(lhs, rhs);
}

Memory* create_object(CVM* vm, std::unordered_map<unsigned int, CMClass*>::iterator code_memory_iter, FunctionFrame* frame, unsigned int constructor_parameter_count) {

	CMClass* code_memory = code_memory_iter->second;

	Memory* memory = new Memory(code_memory_iter);

	gc_nodes.insert(std::make_pair(memory, new Node(memory)));
	vm->gc->increase_gc_counter();

	// run initializer
	run_function(vm, memory, frame, code_memory->initializer, 0);

	// run constructor
	run_function(vm, memory, frame, code_memory->constructor, constructor_parameter_count);

	if (code_memory->get_type() == code_object) {
		// add primitive variables
		bool position_declared = memory->member_variables.find(OBJECT_POSITION) != memory->member_variables.end();
		bool width_declared = memory->member_variables.find(OBJECT_WIDTH) != memory->member_variables.end();
		bool height_declared = memory->member_variables.find(OBJECT_HEIGHT) != memory->member_variables.end();
		bool rotation_declared = memory->member_variables.find(OBJECT_ROTATION) != memory->member_variables.end();
		bool texture_declared = memory->member_variables.find(OBJECT_SPRITE) != memory->member_variables.end();

		if (!position_declared) {
			std::vector<Operand*>* position_data = new std::vector<Operand*>;
			position_data->push_back(new Operand(.0f));
			position_data->push_back(new Operand(.0f));

			memory->member_variables.insert(std::make_pair(OBJECT_POSITION, new Operand(position_data, operand_vector)));
		}

		if (!width_declared) {
			memory->member_variables.insert(std::make_pair(OBJECT_WIDTH, new Operand(100.0f)));
		}
		if (!height_declared) {
			memory->member_variables.insert(std::make_pair(OBJECT_HEIGHT, new Operand(100.0f)));
		}
		if (!rotation_declared) {
			memory->member_variables.insert(std::make_pair(OBJECT_ROTATION, new Operand(.0f)));
		}

		if (!texture_declared) memory->member_variables.insert(std::make_pair(OBJECT_SPRITE, new Operand()));

	}
	return memory;
}

bool compare_operand(Operand* op1, Operand* op2) {

	if (op1->get_type() != op2->get_type()) return false;

	switch (op1->get_type())
	{
	case operand_vector:
		return op1->get_vector_elements() == op2->get_vector_elements();
	case operand_address:
		return op1->get_memory_data() == op2->get_memory_data();
	case operand_bool:
		return op1->get_bool_data() == op2->get_bool_data();
	case operand_number:
		return op1->get_number_data<float>() == op2->get_number_data<float>();
	case operand_integer:
		return op1->get_number_data<int>() == op2->get_number_data<int>();
	case operand_float:
		return op1->get_number_data<float>() == op2->get_number_data<float>();
	case operand_null:
		return op2->data == nullptr;
	case operand_string:
		return op1->get_string_data<std::wstring>() == op2->get_string_data<std::wstring>();
	}

	return op1->data == op2->data;
}

void check_type_for_store(CVM* vm, std::wstring const& type1, std::wstring const& type2) {
	if (type1 == type2) return;

	bool both_object = true;

}

Operand* cast_operand(Operator* op, std::wstring cast_type, Operand* target) {
	Operand* result = nullptr;
	operand_type target_type = target->get_type();
	bool primitive_casted = false;
	Memory* target_memory = nullptr;

	if (cast_type == get_type_name_of_operand(target)) {
		return copy_operand(target);
	}

	if (target->get_type() == operand_address) {
		target_memory = target->get_memory_data();
		if (target_memory->get_cm_class()->name == cast_type)
			return copy_operand(target);
	}

	if (cast_type == L"string") {
		std::wstring target_str;
		switch (target->get_type()) {
		case operand_bool:
			target_str = target->get_bool_data() ? L"true" : L"false";
			break;
		case operand_number:
			target_str = std::to_wstring(target->get_number_data<double>());
			break;
		case operand_integer:
			target_str = std::to_wstring(target->get_number_data<int>());
			break;
		case operand_float:
			target_str = std::to_wstring(target->get_number_data<float>());
			break;
		case operand_null:
			target_str = L"null";
			break;
		case operand_none:
			target_str = L"None";
			break;
		case operand_vector:
			target_str += L"(";
			for (int i = 0; i < target->get_vector_elements()->size(); i++) {
				target_str += std::to_wstring(target->get_vector_elements()->at(i)->get_number_data<float>());
				if (i != target->get_vector_elements()->size() - 1) {
					target_str += L",";
				}
			}
			target_str += L")";
			break;
		}

		result = new Operand(target_str);
	}
	else if (cast_type == L"number") {
		result = new Operand((int)target->size);
		/*
			CHESTNUT_THROW_ERROR(L"Failed to cast " + get_type_string_of_operand(target) + L" into " + cast_type + L" " + target->get_data() + L" is not a number.",
				"FAILED_TO_CAST", "0x12", op->get_line_number());
		*/
	}
	else if (cast_type == L"int") {
		result = new Operand(target->get_number_data<int>());
	}
	else if (cast_type == L"bool") {
		switch (target_type) {
		case operand_number:
			result = new Operand(target->get_number_data<int>() == 1);
			break;
		case operand_string:
			result = new Operand(target->get_string_data<std::wstring>() == L"true");
			break;
		}
	}

	primitive_casted = result != nullptr;

	if (primitive_casted && target_type == operand_address) {
		// tried to cast into primitive, but the target was address (Memory)
		CHESTNUT_THROW_ERROR(L"Failed to cast " + target_memory->get_cm_class()->name + L" into " + cast_type,
			"FAILED_TO_CAST", "0x12", op->get_line_number());
	}

	if (result == nullptr) {
		CHESTNUT_THROW_ERROR(L"Failed to cast " + get_type_name_of_operand(target) + L" into " + cast_type,
			"FAILED_TO_CAST", "0x12", op->get_line_number());
	}


	return result;
}

//#define OPERATOR_TIME_STAMP

void FunctionFrame::run(CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	code_type cm_type = this->get_code_memory()->get_type();

	bool is_builtin = !(cm_type == code_function || cm_type == code_initialize || cm_type == code_constructor);
	if (is_builtin) {
		// not for direct builtin function call.
		return this->run_member_builtin(cm_type, vm, caller, caller_class);
	}

	vm->stack_area.push_back(this);
	std::vector<Operator*> operators = this->code_memory->get_operators();
	size_t operators_size = operators.size();

	for (int line = 0; line < operators_size; line++) {
		Operator* op = operators[line];
		operator_type type = op->get_type();

#ifdef OPERATOR_TIME_STAMP

		double duration;

		auto start = std::chrono::high_resolution_clock::now();

#endif

		//std::wcout << type << " ";

		switch (type) {
		case op_push_string: {
			this->stack->push(new Operand(vm->string_const_pool[op->operands[0]->identifier]));
			break;
		}

		case op_push_number: {
			this->stack->push(new Operand(vm->number_const_pool[op->operands[0]->identifier]));
			break;
		}

		case op_push_integer: {
			this->stack->push(new Operand(vm->integer_const_pool[op->operands[0]->identifier]));
			break;
		}

		case op_push_float: {
			this->stack->push(new Operand(vm->float_const_pool[op->operands[0]->identifier]));
			break;
		}

		case op_push_null: {
			this->stack->push(new Operand());
			break;
		}

		case op_push_this: {
			this->stack->push(create_address_operand(caller_class));
			break;
		}

		case op_push_bool: {
			this->stack->push(new Operand(vm->bool_const_pool[op->operands[0]->identifier]));
			break;
		}

		case op_ret: {
			Operand* op = this->stack->peek();
			this->stack->pop();

			caller->stack->push(op);

			vm->stack_area.pop_back();
			delete this;
			return;
		};

#pragma region OP_INCRE
		case op_incre_global: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = vm->global_area[id];
			found_op->increase();

			break;
		}

		case op_incre_local: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = local_area[id];

			found_op->increase();
			break;
		}

		case op_incre_class: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = caller_class->member_variables[id];

			if (found_op == nullptr) {
				std::wstring var_name = op->operands[0]->identifier;
				CHESTNUT_THROW_ERROR(L"We can\'t find variable named " + std::wstring(var_name.begin(), var_name.end()),
					"RUNTIME_FAILED_TO_LOAD_MEMBER_VARIABLE", "0x05", op->get_line_number());
			}

			found_op->increase();
			break;
		}

		case op_incre_array: {
			Operand* index_peek = this->stack->peek();
			this->stack->pop();
			int index = (index_peek)->get_number_data<int>();

			Operand* array_operand = this->stack->peek(); // do not delete it.
			this->stack->pop();

			ArrayMemory* array_memory = (ArrayMemory*)(array_operand)->data;

			if (array_memory->array_elements->size() <= index) {
				CHESTNUT_THROW_ERROR(L"Array index out of bounds.", "ARRAY_INDEX_OUT_OF_BOUNDS", "0x11", line);
			}

			Operand* result = array_memory->array_elements->at(index);

			result->increase();

			delete array_operand;
			delete index_peek;

			break;
		}
#pragma endregion OP_INCRE

#pragma region OP_DECRE
		case op_decre_global: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = vm->global_area[id];
			found_op->decrease();

			break;
		}

		case op_decre_local: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = local_area[id];

			found_op->decrease();
			break;
		}

		case op_decre_class: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = caller_class->member_variables[id];

			if (found_op == nullptr) {
				std::wstring var_name = op->operands[0]->identifier;
				CHESTNUT_THROW_ERROR(L"We can\'t find variable named " + std::wstring(var_name.begin(), var_name.end()),
					"RUNTIME_FAILED_TO_LOAD_MEMBER_VARIABLE", "0x05", op->get_line_number());
			}

			found_op->decrease();
			break;
		}

		case op_decre_array: {
			Operand* index_peek = this->stack->peek();
			this->stack->pop();
			int index = (index_peek)->get_number_data<int>();

			Operand* array_operand = this->stack->peek(); // do not delete it.
			this->stack->pop();

			ArrayMemory* array_memory = (ArrayMemory*)(array_operand)->data;

			if (array_memory->array_elements->size() <= index) {
				CHESTNUT_THROW_ERROR(L"Array index out of bounds.", "ARRAY_INDEX_OUT_OF_BOUNDS", "0x11", line);
			}

			Operand* result = array_memory->array_elements->at(index);

			result->decrease();

			delete array_operand;
			delete index_peek;

			break;
		}
#pragma endregion OP_DECRE

		case op_for: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			if (condition->get_bool_data()) {
				std::wstring id = op->operands[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			delete condition;

			break;
		}

		case op_new: {
			unsigned int id = op->numeric_operands[0];
			int parameter_count = op->numeric_operands[1];

			Memory* memory = create_object(vm, vm->global_class.find(id), this, parameter_count);

			// store in heap
			vm->heap_area.push_back(memory);

			this->stack->push(create_address_operand(memory));

			break;
		}

		case op_keybaord: {
			std::wstring key = to_upper_all(op->operands[0]->identifier);
			bool result = vm->key_data.find(key) != vm->key_data.end();
			Operand* res_op = new Operand(result);
			this->stack->push(res_op);
			break;
		}

		case op_if: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			if (!condition->get_bool_data()) {
				std::wstring id = op->operands[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			delete condition;

			break;
		}

		case op_not: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			this->stack->push(new Operand(!condition->get_bool_data()));

			delete condition;

			break;
		}

		case op_goto: {
			std::wstring id = op->operands[0]->identifier;
			line = vm->label_id->find(id)->second;

			break;
		}

		case op_add:
		case op_sub:
		case op_mul:
		case op_div:
		case op_pow:
		case op_mod:

		case op_greater:
		case op_lesser:
		case op_equal:
		case op_not_equal:
		case op_eq_lesser:
		case op_eq_greater:

		case op_or:
		case op_and:

		{

			Operand* lhs = this->stack->peek();
			this->stack->pop();

			Operand* rhs = this->stack->peek();
			this->stack->pop();

			// calcaulation with vector and number
			if (rhs->get_type() != lhs->get_type() && !(rhs->is_number_type() && lhs->is_number_type())) {
				if (
					(rhs->get_type() == operand_vector && lhs->is_number_type()) ||
					(lhs->get_type() == operand_vector && rhs->is_number_type())
					) {

					std::vector<Operand*>* calculated_vector = new std::vector<Operand*>;

					Operand* vector_operand = rhs->get_type() == operand_vector ? rhs : lhs;
					Operand* number_operand = rhs->is_number_type() ? rhs : lhs;

					float rhs_v = number_operand->get_number_data<float>();

					std::vector<Operand*>* array_data = vector_operand->get_vector_elements();
					for (int i = 0; i < array_data->size(); i++) {
						float lhs_v = array_data->at(i)->get_number_data<float>();
						float result = 0;
						switch (type) {
						case op_add:
							result = lhs_v + rhs_v;
							break;
						case op_sub:
							result = lhs_v - rhs_v;
							break;
						case op_mul:
							result = lhs_v * rhs_v;
							break;
						case op_div:
							result = lhs_v / rhs_v;
							break;
						case op_pow:
							result = pow(lhs_v, rhs_v);
							break;
						case op_mod:
							result = (int)lhs_v % (int)rhs_v;
							break;
						}
						calculated_vector->push_back(new Operand(result));
					}

					this->stack->push(new Operand(calculated_vector, operand_vector));
				}
				else {
					std::wstring lhs_str = get_type_name_of_operand(rhs);
					std::wstring rhs_str = get_type_name_of_operand(lhs);
					CHESTNUT_THROW_ERROR(L"Failed to calculate " + std::wstring(lhs_str.begin(), lhs_str.end())
						+ L" " + std::wstring(rhs_str.begin(), rhs_str.end()),
						"TRIED_TO_CALCULATE_DIFFERENT_TYPES", "0x11", op->get_line_number());
				}

				delete rhs;
				delete lhs;
				break;
			}

			Operand* result = nullptr;

			switch (type) {
			case op_add: {
				if (rhs->is_number_type()) {
					result = new Operand(lhs->get_number_data<double>() + rhs->get_number_data<double>());
				}
				else if (rhs->get_type() == operand_string) {
					result = new Operand(lhs->get_string_data<std::wstring>() + rhs->get_string_data<std::wstring>());
				}
				else if (rhs->get_type() == operand_vector)
					result = calcaulte_vector_operand(lhs, rhs, cal_add);

				break;
			}
			case op_sub: {
				if (rhs->is_number_type()) {
					result = new Operand(lhs->get_number_data<double>() - rhs->get_number_data<double>());
				}
				else if (rhs->get_type() == operand_vector)
					result = (calcaulte_vector_operand(lhs, rhs, cal_sub));

				break;
			}
			case op_mul: {
				if (rhs->is_number_type()) {
					result = new Operand(lhs->get_number_data<double>() * rhs->get_number_data<double>());
				}
				else if (rhs->get_type() == operand_vector)
					result = calcaulte_vector_operand(lhs, rhs, cal_mult);

				break;
			}
			case op_div:
				if (rhs->is_number_type()) {
					if (rhs->get_number_data<int>() == 0) {
						CHESTNUT_THROW_ERROR(L"Divided by zero. So sorry but We don\'t have Limit system here yet.",
							"RUNTIME_DEVIDED_BY_ZERO", "0x04", op->get_line_number());
					}

					result = new Operand(lhs->get_number_data<double>() / rhs->get_number_data<double>());
				}
				else if (rhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_div));

				break;
			case op_pow:
				if (rhs->is_number_type()) {
					result = new Operand(std::pow(lhs->get_number_data<float>(), rhs->get_number_data<int>()));
				}
				else if (rhs->get_type() == operand_vector)
					result = (calcaulte_vector_operand(lhs, rhs, cal_pow));
				break;
			case op_mod: {
				if (rhs->is_number_type()) {
					if (rhs->get_number_data<int>() == 0) {
						CHESTNUT_THROW_ERROR(L"Divided by zero. So sorry but We don\'t have Limit system here yet.",
							"RUNTIME_DEVIDED_BY_ZERO", "0x04", op->get_line_number());
					}
					result = new Operand(lhs->get_number_data<int>() % rhs->get_number_data<int>());
				}
				else if (rhs->get_type() == operand_vector)
					result = (calcaulte_vector_operand(lhs, rhs, cal_mod));
				break;
			}

			case op_greater: {
				result = new Operand(lhs->get_number_data<double>() < rhs->get_number_data<double>());
				break;
			}

			case op_lesser: {
				result = new Operand(lhs->get_number_data<double>() > rhs->get_number_data<double>());
				break;
			}

			case op_eq_lesser: {
				result = new Operand(lhs->get_number_data<double>() >= rhs->get_number_data<double>());
				break;
			}

			case op_eq_greater: {
				result = new Operand(lhs->get_number_data<double>() <= rhs->get_number_data<double>());
				break;
			}

			case op_equal: {
				result = new Operand(compare_operand(rhs, lhs));
				break;
			}

			case op_not_equal: {
				result = new Operand(!compare_operand(rhs, lhs));
				break;
			}

			case op_or: {
				bool _lhs = rhs->get_bool_data();
				bool _rhs = lhs->get_bool_data();

				result = new Operand(_lhs || _rhs);

				break;
			}

			case op_and: {
				bool _lhs = rhs->get_bool_data();
				bool _rhs = lhs->get_bool_data();

				result = new Operand(_lhs && _rhs);

				break;
			}
			}

			this->stack->push(result);

			delete rhs;
			delete lhs;
			break;
		}

		case op_store_global: {
			Operand* peek_op = this->stack->peek();
			Operand* peek = copy_operand(peek_op);
			this->stack->pop();
			unsigned int id = op->numeric_operands[0];

			std::wstring name = op->operands[0]->identifier;

			if (vm->global_area.find(id) != vm->global_area.end())
				vm->global_area[id] = peek;
			else
				vm->global_area.insert(std::make_pair(id, peek));

			vm->global_area[id]->variable_name = name;

			if (peek->get_type() == operand_address) {
				// modifying nodes for attr
				gc_nodes[caller_class]->childs.push_back(gc_nodes[peek->get_memory_data()]);
			}

			delete peek_op;

			break;
		}

		case op_store_class: {
			Operand* peek_op = this->stack->peek();
			Operand* peek = copy_operand(peek_op);
			this->stack->pop();
			unsigned int id = op->numeric_operands[0];

			std::wstring name = op->operands[0]->identifier;

			peek->variable_name = name;

			if (peek->get_type() == operand_address) {
				// disconnect node between already assigned varaible
				if (caller_class->member_variables.find(id) != caller_class->member_variables.end()) {
					Memory* already_assigned_memory = (Memory*)caller_class->member_variables[id]->data;

					disconnectNode(caller_class, already_assigned_memory);
				}

				// modifying nodes for attr
				Memory* child_memory = (Memory*)(peek->data);
				gc_nodes[caller_class]->childs.push_back(gc_nodes[child_memory]);
			}

			if (caller_class->member_variables.find(id) == caller_class->member_variables.end()) {
				//std::cout << "new variable declared." << op->operands[0]->identifier << std::endl;
				caller_class->member_variables.insert(std::make_pair(id, peek));
				caller_class->member_variable_names.insert(std::make_pair(id, op->operands[0]->identifier));
			}
			else {
				delete caller_class->member_variables[id];
				caller_class->member_variables[id] = peek;
				caller_class->member_variable_names[id] = op->operands[0]->identifier;
			}

			delete peek_op;

			break;
		}

		case op_store_local: {
			Operand* peek_op = this->stack->peek();
			Operand* peek = copy_operand(peek_op);
			this->stack->pop();
			unsigned int id = op->numeric_operands[0];

			if (local_area.contains(id))
				delete local_area[id];

			local_area[id] = peek;

			//check_type_for_store(vm, get_type_string_of_operand(peek), type);

			delete peek_op;

			break;
		}

		case op_store_attr: {
			Operand* attr_target = this->stack->peek();
			this->stack->pop();

			Operand* store_value_op = this->stack->peek();
			Operand* store_value = copy_operand(store_value_op);
			this->stack->pop();

			unsigned int store_id = op->numeric_operands[0];

			operand_type op_type = attr_target->get_type();

			if (op_type == operand_address) {
				Memory* attr_memory = attr_target->get_memory_data();
				std::wstring name = L"";

				if (attr_memory->member_variables.find(store_id) != attr_memory->member_variables.end()) {
					if (attr_memory->member_variables[store_id]->get_type() == operand_address) {
						disconnectNode(attr_memory, (Memory*)attr_memory->member_variables[store_id]->data);
					}

					name = attr_memory->member_variables[store_id]->variable_name;
					delete attr_memory->member_variables[store_id];
					attr_memory->member_variables.erase(attr_memory->member_variables.find(store_id));
				}

				attr_memory->member_variables.insert(std::make_pair(store_id, store_value));
				attr_memory->member_variables[store_id]->variable_name = name;

				// modifying nodes for attr
				if (store_value->get_type() == operand_address) {
					gc_nodes[attr_memory]->childs.push_back(gc_nodes[(Memory*)store_value->data]);
				}
			}
			else if (op_type == operand_vector) {
				std::wstring name = op->operands[0]->identifier;
				CHESTNUT_THROW_ERROR(L"Unable to store value into " + std::wstring(name.begin(), name.end()) + L" because it is vector varaible. it is read-only",
					"RUNTIME_FAILED_TO_STORE_VALUE_INTO_VECTOR", "0x09", op->get_line_number());
			}

			delete attr_target;
			delete store_value_op;

			break;
		}

		case op_array_get: {
			Operand* index_peek = this->stack->peek();
			this->stack->pop();
			int index = (index_peek)->get_number_data<int>();

			Operand* array_operand = this->stack->peek(); // do not delete it.
			this->stack->pop();

			ArrayMemory* array_memory = (ArrayMemory*)(array_operand)->data;

			if (array_memory->array_elements->size() <= index) {
				CHESTNUT_THROW_ERROR(L"Array index out of bounds.", "ARRAY_INDEX_OUT_OF_BOUNDS", "0x11", line);
			}

			Operand* result = array_memory->array_elements->at(index);

			this->stack->push(copy_operand(result));

			delete array_operand;
			delete index_peek;

			break;
		}

		case op_array: {
			int array_size = op->numeric_operands[0];

			Operand* result = nullptr;
			std::vector<Operand*>* elements = new std::vector<Operand*>;

			for (int i = 0; i < array_size; i++) {
				Operand* peek = this->stack->peek();
				this->stack->pop();

				elements->push_back(peek);
			}

			ArrayMemory* memory = new ArrayMemory(vm->builtin_class.find(vm->array_code_memory_id), elements);

			// store in heap
			vm->heap_area.push_back(memory);
			gc_nodes.insert(std::make_pair(memory, new Node(memory)));

			vm->gc->increase_gc_counter();
			this->stack->push(create_address_operand(memory));

			break;
		}

		case op_vector: {
			int vector_size = op->numeric_operands[0];

			Operand* result = nullptr;
			std::vector<Operand*>* elements = new std::vector<Operand*>;

			for (int i = 0; i < vector_size; i++) {
				Operand* _peek = this->stack->peek();
				this->stack->pop();

				elements->push_back(_peek);
			}

			result = new Operand(elements, operand_vector);

			this->stack->push(result);

			break;
		}

		case op_load_global: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = vm->global_area[id];

			this->stack->push(copy_operand(found_op));

			break;
		}

		case op_load_local: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = local_area[id];

			this->stack->push(copy_operand(found_op));
			break;
		}

		case op_load_class: {
			unsigned int id = op->numeric_operands[0];
			Operand* found_op = caller_class->member_variables[id];

			if (found_op == nullptr) {
				std::wstring var_name = op->operands[0]->identifier;
				CHESTNUT_THROW_ERROR(L"We can\'t find variable named " + std::wstring(var_name.begin(), var_name.end()),
					"RUNTIME_FAILED_TO_LOAD_MEMBER_VARIABLE", "0x05", op->get_line_number());
			}

			this->stack->push(copy_operand(found_op));
			break;
		}

		case op_call_class: {
			unsigned int id = op->numeric_operands[0];
			int parameter_count = op->numeric_operands[1];

			CMFunction* code_memory = caller_class->get_cm_class()->member_functions->find(id)->second;

			run_function(vm, caller_class, this, code_memory, parameter_count);

			break;
		}

		case op_cast: {
			Operand* target = this->stack->peek();
			this->stack->pop();

			std::wstring cast_type = op->operands[0]->identifier;
			this->stack->push(cast_operand(op, cast_type, target));

			delete target;

			break;
		}

		case op_call_global: {
			unsigned int id = op->numeric_operands[0];
			int parameter_count = op->numeric_operands[1];
			CMFunction* code_memory = vm->global_functions[id];

			run_function(vm, nullptr, this, code_memory, parameter_count);

			break;
		}

		case op_super_call: {
			int parameter_count = op->numeric_operands[0];
			unsigned int parent_id = caller_class->get_cm_class()->get_parent_id();

			CMClass* parent_code_memory = vm->global_class[parent_id];
			CMFunction* parent_constructor = parent_code_memory->constructor;
			CMFunction* parent_initializer = parent_code_memory->initializer;

			run_function(vm, caller_class, this, parent_initializer, 0);

			run_function(vm, caller_class, this, parent_constructor, parameter_count);

			std::unordered_map<unsigned int, CMFunction*>::iterator member_function_iter = parent_code_memory->member_functions->begin();

			for (; member_function_iter != parent_code_memory->member_functions->end(); member_function_iter++) {
				if (member_function_iter->second->get_access_modifier() != L"private")
					caller_class->get_cm_class()->member_functions->insert(std::make_pair(member_function_iter->second->get_id(), member_function_iter->second));
			}

			break;
		}

		case op_call_builtin: {
			this->run_builtin(op, vm, this, caller_class);
			break;
		}

		case op_load_attr: {
			Operand* target = this->stack->peek();
			this->stack->pop();
			operand_type type = target->get_type();
			Operand* found_op = nullptr;

			if (target->get_type() == operand_null) {
				std::wstring name = op->operands[0]->identifier;

				CHESTNUT_THROW_ERROR(L"Failed to load " + std::wstring(name.begin(), name.end()) + L" because target was null.",
					"RUNTIME_LOAD_FROM_NULL", "0x07", op->get_line_number());
			}

			if (type == operand_address) {
				Memory* memory = (Memory*)target->data;
				found_op = memory->member_variables[op->numeric_operands[0]];
			}
			else if (type == operand_vector) {
				int index = op->numeric_operands[0];

				found_op = target->get_vector_elements()->at(index);
			}

			this->stack->push(copy_operand(found_op));

			delete target;

			break;
		}

		case op_call_attr: {
			unsigned int id = op->numeric_operands[0];
			int parameter_count = op->numeric_operands[1];

			std::stack<Operand*> parameter_store_stack;

			for (int i = 0; i < parameter_count; i++) {
				parameter_store_stack.push(this->stack->peek());
				this->stack->pop();
			}

			Operand* target = this->stack->peek();
			this->stack->pop();

			for (int i = 0; i < parameter_count; i++) {
				this->stack->push(parameter_store_stack.top());
				parameter_store_stack.pop();
			}

			if (target->get_type() == operand_null) {
				std::wstring name = op->operands[0]->identifier;
				CHESTNUT_THROW_ERROR(L"Failed to call " + std::wstring(name.begin(), name.end()) + L" because target was null.",
					"RUNTIME_CALL_FROM_NULL", "0x01", op->get_line_number());
			}

			Memory* memory = (Memory*)(target->data);
			CMClass* cm = memory->get_cm_class();
			CMFunction* callee_function = (CMFunction*)cm->member_functions->find(id)->second;

			run_function(vm, memory, this, callee_function, parameter_count);

			delete target;

			break;
		}

		}

#ifdef OPERATOR_TIME_STAMP
		auto finish = std::chrono::high_resolution_clock::now();

		std::wcout << std::endl;
		std::wstring diff = std::to_wstring(std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count());
		CHESTNUT_LOG(L"Operator " + std::to_wstring(op->get_type()) + L" end with : " + std::wstring(diff.begin(), diff.end()) + L"ns.", log_level::log_default);
#endif

	}

	vm->stack_area.pop_back();
	delete this;
}