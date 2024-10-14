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

const std::wstring get_type_string_of_operand(Operand* op) {
	std::wstring type_string_operand = L"";

	Operand* extracted_op = extract_value_of_opernad(op);

	switch (extracted_op->get_type()) {
	case operand_number:
		type_string_operand = L"number";
		break;
	case operand_address:
		type_string_operand
			= reinterpret_cast<Memory*>(std::stoull(extracted_op->get_data()))->get_cm_class()->name;
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

void run_function(CVM* vm, Memory* caller_class, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count) {
	FunctionFrame* frame = new FunctionFrame(code_memory);

	bool is_defined_function
		= code_memory->get_type() == code_function || code_memory->get_type() == code_constructor || code_memory->get_type() == code_initialize;

	if (is_defined_function && parameter_count != code_memory->get_param_types().size()) {
		std::wstring name = caller_frame->get_code_memory()->name;
		CHESTNUT_THROW_ERROR(L"Failed to call " + std::wstring(name.begin(), name.end()) + L". You pass the wrong parameters",
			"RUNTIME_WRONG_PARAMETER", "0x02", 0);
	}

	if (is_defined_function) {
		for (int i = 0; i < parameter_count; i++) {
			Operand* op = caller_frame->stack->peek();
			caller_frame->stack->pop();

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

			frame->local_area.insert(std::make_pair(i, copy_operand(op)));
		}
	}
	else {
		std::vector<Operand*> array_params;
		for (int i = 0; i < parameter_count; i++) {
			Operand* op = caller_frame->stack->peek();
			caller_frame->stack->pop();
			array_params.push_back(op);
		}

		for (int i = 0; i < array_params.size(); i++) {
			frame->stack->push(copy_operand(array_params[array_params.size() - i - 1]));
		}
	}

	frame->run(vm, caller_frame, caller_class);
}

Operand* calcaulte_vector_operand(Operand* lhs, Operand* rhs, double (*cal)(double l, double r)) {
	Operand* lhs_vector = lhs;
	Operand* rhs_vector = rhs;

	std::vector<Operand*>* calculated_result = new std::vector<Operand*>;

	size_t min_vector_size = (size_t)min(lhs_vector->get_vector_elements()->size(), rhs_vector->get_vector_elements()->size());
	for (int i = 0; i < min_vector_size; i++) {
		double lhs_v = std::stod(lhs_vector->get_vector_elements()->at(i)->get_data());
		double rhs_v = std::stod(rhs_vector->get_vector_elements()->at(i)->get_data());

		double calculated_v = cal(lhs_v, rhs_v);

		calculated_result->push_back(new Operand(std::to_wstring(calculated_v), operand_number));
	}

	return new Operand(calculated_result, operand_vector);
}

double cal_add(double lhs, double rhs) {
	return lhs + rhs;
}

double cal_sub(double lhs, double rhs) {
	return lhs - rhs;
}

double cal_mult(double lhs, double rhs) {
	return lhs * rhs;
}

double cal_div(double lhs, double rhs) {
	if (rhs != 0) {
		return lhs / rhs;
	}
	return 0;
}

double cal_mod(double lhs, double rhs) {
	if (rhs != 0) {
		return (int)lhs % (int)rhs;
	}
	return 0;
}

double cal_pow(double lhs, double rhs) {
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
			position_data->push_back(new Operand(L"0", operand_number));
			position_data->push_back(new Operand(L"0", operand_number));
			memory->member_variables.insert(std::make_pair(OBJECT_POSITION, new Operand(position_data, operand_vector)));
		}

		if (!width_declared) memory->member_variables.insert(std::make_pair(OBJECT_WIDTH, new Operand(L"100", operand_number)));
		if (!height_declared) memory->member_variables.insert(std::make_pair(OBJECT_HEIGHT, new Operand(L"100", operand_number)));
		if (!rotation_declared) memory->member_variables.insert(std::make_pair(OBJECT_ROTATION, new Operand(L"0", operand_number)));
		if (!texture_declared) memory->member_variables.insert(std::make_pair(OBJECT_SPRITE, new Operand(L"", operand_null)));

	}
	return memory;
}

bool operand_compare(Operand* op1, Operand* op2) {

	if (op1->get_type() != op2->get_type()) return false;

	if (op1->get_type() == operand_vector) {
		return op1->get_vector_elements() == op2->get_vector_elements();
	}

	if (op1->get_type() == operand_address) {
		return
			reinterpret_cast<Memory*>(std::stoull(op1->get_data())) ==
			reinterpret_cast<Memory*>(std::stoull(op2->get_data()));
	}

	return op1 == op2;
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

	if (cast_type == get_type_string_of_operand(target)) {
		return target;
	}

	if (target->get_type() == operand_address) {
		target_memory = reinterpret_cast<Memory*>(std::stoull(target->get_data()));
		if (target_memory->get_cm_class()->name == cast_type)
			return target;
	}

	if (cast_type == L"string") {
		result = new Operand(target->get_data(), operand_string);
	}
	else if (cast_type == L"number") {
		std::wstring target_number_string = target->get_data();
		bool is_number = true;

		for (int i = 0; i < target_number_string.size(); i++) {
			if (!(iswdigit(target_number_string[i]) || target_number_string[i] == '.' || target_number_string[i] == 'f')) {
				is_number = false;
			}
		}

		if (is_number) {
			result = new Operand(target->get_data(), operand_number);
		}
		else {
			CHESTNUT_THROW_ERROR(L"Failed to cast " + get_type_string_of_operand(target) + L" into " + cast_type + L" " + target->get_data() + L" is not a number.",
				"FAILED_TO_CAST", "0x12", op->get_line_number());
		}
	}
	else if (cast_type == L"bool") {
		switch (target_type) {
		case operand_number:
			result = new Operand(target->get_data() == L"1" ? L"true" : L"false", operand_bool);
			break;
		case operand_string:
			result = new Operand(target->get_data() == L"true" ? L"true" : L"false", operand_bool);
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
		CHESTNUT_THROW_ERROR(L"Failed to cast " + get_type_string_of_operand(target) + L" into " + cast_type,
			"FAILED_TO_CAST", "0x12", op->get_line_number());
	}

	return result;
}

void FunctionFrame::object_builtin_render(CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	CMObject* caller_object = (CMObject*)caller_class->get_cm_class();

	Memory* shader_memory = reinterpret_cast<Memory*>(std::stoull(vm->global_area[SHADER_MEMORY]->get_data()));
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	Operand* texture_op = caller_class->member_variables[OBJECT_SPRITE];

	if (texture_op->get_type() == operand_null) {
		std::wstring name = caller_class->get_cm_class()->name;
		CHESTNUT_THROW_ERROR(L"Failed to render " + std::wstring(name.begin(), name.end()) + L". You must assing texture for it.",
			"RUNTIME_NO_TEXTURE_FOR_OBJECT", "0x03", 0);
	}

	Operand* position_op = extract_value_of_opernad(caller_class->member_variables[OBJECT_POSITION]);

	float _x = std::stof(extract_value_of_opernad(position_op->get_vector_elements()->at(0))->get_data()),
		_y = std::stof(extract_value_of_opernad(position_op->get_vector_elements()->at(1))->get_data());

	Operand* width_op = caller_class->member_variables[OBJECT_WIDTH];
	Operand* height_op = caller_class->member_variables[OBJECT_HEIGHT];

	float f_width = std::stof(extract_value_of_opernad(width_op)->get_data())
		, f_height = std::stof(extract_value_of_opernad(height_op)->get_data());

	Operand* rotation_op = caller_class->member_variables[OBJECT_ROTATION];
	float f_rotation = std::stof(extract_value_of_opernad(rotation_op)->get_data());

	std::unordered_map<std::wstring, CMImage*>::iterator image_data_iter = vm->resources.find(extract_value_of_opernad(texture_op)->get_data());

	assert(image_data_iter != vm->resources.end());

	CMImage* image_data = image_data_iter->second;

	render_image(shader_cm, image_data->get_texture(), image_data->get_vao(),
		_x, _y, f_width, f_height, f_rotation, vm->proj_width, vm->proj_height);

	delete this;
	return;
}

//#define OPERATOR_TIME_STAMP

void FunctionFrame::run(CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	code_type cm_type = this->get_code_memory()->get_type();
	if (cm_type == code_render) {
		return object_builtin_render(vm, caller, caller_class);
	}

	if (cm_type == code_array_push) {
		Operand* _target_element = this->stack->peek();
		this->stack->pop();

		Operand* target_element = extract_value_of_opernad(_target_element);

		((ArrayMemory*)caller_class)->array_elements->push_back(target_element);

		if (target_element->get_type() == operand_address) {
			// modifying nodes for attr
			gc_nodes[caller_class]->childs.push_back(gc_nodes[reinterpret_cast<Memory*>(std::stoull(target_element->get_data()))]);
		}

		delete this;
		return;
	}

	if (cm_type == code_array_size) {
		Operand* result = new Operand(std::to_wstring(((ArrayMemory*)caller_class)->array_elements->size()), operand_number);
		caller->stack->push(result);

		delete this;
		return;
	}

	if (cm_type == code_array_remove) {

		Operand* _target_element = this->stack->peek();
		this->stack->pop();

		Operand* target_element = extract_value_of_opernad(_target_element);
		std::vector<Operand*>* _array = ((ArrayMemory*)caller_class)->array_elements;

		int index = 0;

		for (Operand* _element : (*_array)) {
			if (operand_compare(extract_value_of_opernad(_element), target_element)) {
				break;
			}
			index++;
		}

		if (index == _array->size()) { // Failed to find element in array.
			delete this;
			return;
		}

		if (target_element->get_type() == operand_address) {
			disconnectNode(caller_class, reinterpret_cast<Memory*>(std::stoull(target_element->get_data())));
		}

		_array->erase(_array->begin() + index);

		delete this;
		return;
	}

	if (cm_type == code_array_set) {
		Operand* _target_element = this->stack->peek();
		this->stack->pop();

		Operand* _index = this->stack->peek();
		this->stack->pop();

		Operand* target_element = extract_value_of_opernad(_target_element);
		_index = extract_value_of_opernad(_index);

		std::vector<Operand*>* _array = ((ArrayMemory*)caller_class)->array_elements;

		int index = std::stoi(_index->get_data());

		if (_array->at(index)->get_type() == operand_address) {
			disconnectNode(caller_class, reinterpret_cast<Memory*>(std::stoull(_array->at(index)->get_data())));
		}

		_array->at(index) = target_element;

		if (target_element->get_type() == operand_address) {
			// modifying nodes for attr
			gc_nodes[caller_class]->childs.push_back(gc_nodes[reinterpret_cast<Memory*>(std::stoull(target_element->get_data()))]);
		}

		delete this;
		return;
	}

	vm->stack_area.push_back(this);
	std::vector<Operator*> operators = this->code_memory->get_operators();

	for (int line = 0; line < operators.size(); line++) {
		Operator* op = operators[line];
		operator_type type = op->get_type();

#ifdef OPERATOR_TIME_STAMP

		clock_t start, finish;
		double duration;

		start = clock();

#endif
		switch (type) {
		case op_push_string: {
			std::wstring data = op->operands[0]->identifier;
			data = data.substr(1, data.size() - 2);

			this->stack->push(new Operand(data, operand_string));
			break;
		}

		case op_push_number: {
			this->stack->push(new Operand(op->operands[0]->identifier, operand_number));
			break;
		}

		case op_push_null: {
			this->stack->push(new Operand(L"", operand_null));
			break;
		}

		case op_push_this: {
			this->stack->push(create_address_operand(caller_class));
			break;
		}

		case op_push_bool: {
			this->stack->push(new Operand(op->operands[0]->identifier, operand_bool));
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

		case op_for: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			if (extract_value_of_opernad(condition)->get_data() == L"true") {
				std::wstring id = op->operands[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			delete condition;

			break;
		}

		case op_new: {
			unsigned int id = std::stoi(op->operands[0]->identifier);
			int parameter_count = std::stoi(op->operands[1]->identifier);

			Memory* memory = create_object(vm, vm->global_class.find(id), this, parameter_count);

			// store in heap
			vm->heap_area.push_back(memory);

			this->stack->push(create_address_operand(memory));

			break;
		}

		case op_keybaord: {
			std::wstring key = to_upper_all(op->operands[0]->identifier);
			std::wstring result = vm->key_data.find(key) != vm->key_data.end() ? L"true" : L"false";

			this->stack->push(new Operand(result, operand_bool));
			break;
		}

		case op_if: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			if (extract_value_of_opernad(condition)->get_data() == L"false") {
				std::wstring id = op->operands[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			delete condition;

			break;
		}

		case op_inc: {
			Operand* target = extract_value_of_opernad(this->stack->peek());
			this->stack->pop();

			double v = std::stod(target->get_data()) + 1;
			target->set_data(std::to_wstring(v));

			break;
		}

		case op_dec: {
			Operand* target = extract_value_of_opernad(this->stack->peek());
			this->stack->pop();

			double v = std::stod(target->get_data()) - 1;
			target->set_data(std::to_wstring(v));

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
			Operand* lhs_op = this->stack->peek();
			this->stack->pop();

			Operand* rhs_op = this->stack->peek();
			this->stack->pop();

			Operand* lhs = extract_value_of_opernad(lhs_op);
			Operand* rhs = extract_value_of_opernad(rhs_op);

			if (lhs->get_type() != rhs->get_type()) {

				if (
					(lhs->get_type() == operand_vector && rhs->get_type() == operand_number) ||
					(rhs->get_type() == operand_vector && lhs->get_type() == operand_number)
					) {

					std::vector<Operand*>* calculated_vector = new std::vector<Operand*>;

					Operand* vector_operand = lhs->get_type() == operand_vector ? lhs : rhs;
					Operand* number_operand = lhs->get_type() == operand_number ? lhs : rhs;

					double rhs_v = std::stod(number_operand->get_data());
					std::vector<Operand*>* array_data = vector_operand->get_vector_elements();
					for (int i = 0; i < array_data->size(); i++) {
						double lhs_v = std::stod(array_data->at(i)->get_data());

						switch (type) {
						case op_add:
							calculated_vector->push_back(new Operand(std::to_wstring(lhs_v + rhs_v), operand_number));
							break;
						case op_sub:
							calculated_vector->push_back(new Operand(std::to_wstring(lhs_v - rhs_v), operand_number));
							break;
						case op_mul:
							calculated_vector->push_back(new Operand(std::to_wstring(lhs_v * rhs_v), operand_number));
							break;
						case op_div:
							calculated_vector->push_back(new Operand(std::to_wstring(lhs_v / rhs_v), operand_number));
							break;
						case op_pow:
							calculated_vector->push_back(new Operand(std::to_wstring(pow(lhs_v, rhs_v)), operand_number));
							break;
						case op_mod:
							calculated_vector->push_back(new Operand(std::to_wstring((int)lhs_v % (int)rhs_v), operand_number));
							break;
						}
					}

					this->stack->push(new Operand(calculated_vector, operand_vector));
				}
				else {
					std::wstring lhs_str = get_type_string_of_operand(lhs);
					std::wstring rhs_str = get_type_string_of_operand(rhs);
					CHESTNUT_THROW_ERROR(L"Failed to calculate " + std::wstring(lhs_str.begin(), lhs_str.begin())
						+ L" " + std::wstring(rhs_str.begin(), rhs_str.begin()),
						"TRIED_TO_CALCULATE_DIFFERENT_TYPES", "0x11", op->get_line_number());
				}

				delete lhs_op;
				delete rhs_op;
				break;
			}

			switch (type) {
			case op_add:
				if (lhs->get_type() == operand_number)
					this->stack->push(new Operand(std::to_wstring(
						std::stod(lhs->get_data())
						+ std::stod(rhs->get_data())), operand_number));
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_add));

				break;
			case op_sub:
				if (lhs->get_type() == operand_number)
					this->stack->push(new Operand(std::to_wstring(
						std::stod(lhs->get_data())
						- std::stod(rhs->get_data())), operand_number));
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_sub));

				break;
			case op_mul:
				if (lhs->get_type() == operand_number)
					this->stack->push(new Operand(std::to_wstring(
						std::stod(lhs->get_data())
						* std::stod(rhs->get_data())), operand_number));
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_mult));

				break;
			case op_div:
				if (lhs->get_type() == operand_number) {
					if (std::stod(rhs->get_data()) == 0) {
						CHESTNUT_THROW_ERROR(L"Divided by zero. So sorry but We don\'t have Limit system here yet.",
							"RUNTIME_DEVIDED_BY_ZERO", "0x04", op->get_line_number());
					}

					this->stack->push(new Operand(std::to_wstring(
						std::stod(lhs->get_data())
						/ std::stod(rhs->get_data())), operand_number));

				}
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_div));

				break;
			case op_pow:
				if (lhs->get_type() == operand_number) {
					this->stack->push(new Operand(std::to_wstring(
						pow(std::stod(lhs->get_data()), std::stod(rhs->get_data()))), operand_number));
				}
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_pow));
				break;
			case op_mod: {
				if (lhs->get_type() == operand_number) {
					if (rhs == 0) {
						CHESTNUT_THROW_ERROR(L"Divided by zero. So sorry but We don\'t have Limit system here yet.",
							"RUNTIME_DEVIDED_BY_ZERO", "0x04", op->get_line_number());
					}
					this->stack->push(new Operand(std::to_wstring(
						(int)std::stod(lhs->get_data())
						% (int)std::stod(rhs->get_data())), operand_number));
				}
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_mod));
				break;
			}

			case op_greater: {
				bool result = std::stod(lhs->get_data()) < std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));
				break;
			}

			case op_lesser: {
				bool result = std::stod(lhs->get_data()) > std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));
				break;
			}

			case op_eq_lesser: {
				bool result = std::stod(lhs->get_data()) >= std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));
				break;
			}

			case op_eq_greater: {
				bool result = std::stod(lhs->get_data()) <= std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));
				break;
			}

			case op_equal: {
				bool result = lhs->get_data() == rhs->get_data();

				if (lhs->get_type() == operand_number)
					result = std::stod(lhs->get_data()) == std::stod(rhs->get_data());

				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));
				break;
			}

			case op_not_equal: {
				bool result = lhs->get_data() != rhs->get_data();
				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));
				break;
			}

			case op_or: {
				bool _lhs = lhs->get_data() == L"true";
				bool _rhs = rhs->get_data() == L"true";
				bool result = _lhs || _rhs;

				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));

				break;
			}

			case op_and: {
				bool _lhs = lhs->get_data() == L"true";
				bool _rhs = rhs->get_data() == L"true";
				bool result = _lhs && _rhs;

				this->stack->push(new Operand(result ? L"true" : L"false", operand_bool));
				break;
			}
			}
			delete lhs_op;
			delete rhs_op;
			break;
		}

		case op_store_global: {
			Operand* peek_op = this->stack->peek();
			Operand* peek = copy_operand(peek_op);
			this->stack->pop();
			unsigned int id = std::stoi(op->operands[0]->identifier);

			std::wstring name = op->operands[1]->identifier;

			if (vm->global_area.find(id) != vm->global_area.end())
				vm->global_area[id] = peek;
			else
				vm->global_area.insert(std::make_pair(id, peek));

			vm->global_area[id]->variable_name = name;

			if (peek->get_type() == operand_address) {
				// modifying nodes for attr
				gc_nodes[caller_class]->childs.push_back(gc_nodes[reinterpret_cast<Memory*>(std::stoull(peek->get_data()))]);
			}

			delete peek_op;

			break;
		}

		case op_store_class: {
			Operand* peek_op = this->stack->peek();
			Operand* peek = copy_operand(peek_op);
			this->stack->pop();
			unsigned int id = std::stoi(op->operands[0]->identifier);

			std::wstring name = op->operands[1]->identifier;

			peek->variable_name = name;

			if (peek->get_type() == operand_address) {
				// disconnect node between already assigned varaible
				if (caller_class->member_variables.find(id) != caller_class->member_variables.end()) {
					Memory* already_assigned_memory = reinterpret_cast<Memory*>(std::stoull(caller_class->member_variables[id]->get_data()));

					disconnectNode(caller_class, already_assigned_memory);
				}

				// modifying nodes for attr
				Memory* chlid_memory = reinterpret_cast<Memory*>(std::stoull(peek->get_data()));
				gc_nodes[caller_class]->childs.push_back(gc_nodes[chlid_memory]);
			}

			if (caller_class->member_variables.find(id) == caller_class->member_variables.end()) {
				//std::cout << "new variable declared." << op->operands[0]->identifier << std::endl;
				caller_class->member_variables.insert(std::make_pair(id, peek));
				caller_class->member_variable_names.insert(std::make_pair(id, op->operands[1]->identifier));
			}
			else {
				delete caller_class->member_variables[id];
				caller_class->member_variables[id] = peek;
				caller_class->member_variable_names[id] = op->operands[1]->identifier;
			}

			delete peek_op;

			break;
		}

		case op_store_local: {
			Operand* peek_op = this->stack->peek();
			Operand* peek = copy_operand(peek_op);
			this->stack->pop();
			unsigned int id = std::stoi(op->operands[0]->identifier);

			std::wstring name = L"";
			std::wstring type = L"";

			if (local_area.find(id) != local_area.end()) {
				delete local_area[id];
				local_area[id] = peek;
				name = local_area[id]->variable_name;
				type = get_type_string_of_operand(local_area[id]);
			}
			else {
				local_area.insert(std::make_pair(id, peek));
			}

			local_area[id]->variable_name = name;

			//check_type_for_store(vm, get_type_string_of_operand(peek), type);

			delete peek_op;

			break;
		}

		case op_store_attr: {
			Operand* attr_target_op = this->stack->peek();
			Operand* attr_target = extract_value_of_opernad(attr_target_op);
			this->stack->pop();

			Operand* store_value_op = this->stack->peek();
			Operand* store_value = copy_operand(store_value_op);
			this->stack->pop();

			unsigned int store_id = std::stoi(op->operands[0]->identifier);

			operand_type op_type = attr_target->get_type();

			if (op_type == operand_address) {
				Memory* attr_memory = reinterpret_cast<Memory*>(std::stoull(attr_target->get_data()));
				std::wstring name = L"";

				if (attr_memory->member_variables.find(store_id) != attr_memory->member_variables.end()) {

					if (attr_memory->member_variables[store_id]->get_type() == operand_address) {
						disconnectNode(attr_memory, reinterpret_cast<Memory*>(std::stoull(attr_memory->member_variables[store_id]->get_data())));
					}

					name = attr_memory->member_variables[store_id]->variable_name;
					delete attr_memory->member_variables[store_id];
					attr_memory->member_variables.erase(attr_memory->member_variables.find(store_id));
				}

				attr_memory->member_variables.insert(std::make_pair(store_id, store_value));
				attr_memory->member_variables[store_id]->variable_name = name;

				// modifying nodes for attr
				if (store_value->get_type() == operand_address) {
					gc_nodes[attr_memory]->childs.push_back(gc_nodes[reinterpret_cast<Memory*>(std::stoull(store_value->get_data()))]);
				}
			}
			else if (op_type == operand_vector) {
				std::wstring name = op->operands[1]->identifier;
				CHESTNUT_THROW_ERROR(L"Unable to store value into " + std::wstring(name.begin(), name.end()) + L" because it is vector varaible. it is read-only",
					"RUNTIME_FAILED_TO_STORE_VALUE_INTO_VECTOR", "0x09", op->get_line_number());
			}

			delete attr_target_op;
			delete store_value_op;

			break;
		}

		case op_array_get: {
			Operand* index_peek = this->stack->peek();
			this->stack->pop();
			int index = std::stoi(extract_value_of_opernad(index_peek)->get_data());

			Operand* array_operand = this->stack->peek(); // do not delete it.
			this->stack->pop();

			ArrayMemory* array_memory = reinterpret_cast<ArrayMemory*>(std::stoull(extract_value_of_opernad(array_operand)->get_data()));

			Operand* result = array_memory->array_elements->at(index);

			this->stack->push(create_op_address_operand(result));

			delete array_operand;
			delete index_peek;

			break;
		}

		case op_array: {
			int array_size = std::stoi(op->operands[0]->identifier);

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
			int vector_size = std::stoi(op->operands[0]->identifier);

			Operand* result = nullptr;
			std::vector<Operand*>* elements = new std::vector<Operand*>;

			for (int i = 0; i < vector_size; i++) {
				Operand* _peek = this->stack->peek();
				Operand* extracted_data = extract_value_of_opernad(_peek);
				this->stack->pop();

				elements->push_back(extracted_data);
			}

			result = new Operand(elements, operand_vector);

			this->stack->push(result);

			break;
		}

		case op_load_global: {
			unsigned int id = std::stoi(op->operands[0]->identifier);
			Operand* found_op = vm->global_area[id];

			this->stack->push(copy_operand(found_op));

			break;
		}

		case op_load_local: {
			unsigned int id = std::stoi(op->operands[0]->identifier);
			Operand* found_op = local_area[id];

			this->stack->push(copy_operand(found_op));
			break;
		}

		case op_load_class: {
			unsigned int id = std::stoi(op->operands[0]->identifier);
			Operand* found_op = caller_class->member_variables[id];

			if (found_op == nullptr) {
				std::wstring var_name = op->operands[1]->identifier;
				CHESTNUT_THROW_ERROR(L"We can\'t find variable named " + std::wstring(var_name.begin(), var_name.end()),
					"RUNTIME_FAILED_TO_LOAD_MEMBER_VARIABLE", "0x05", op->get_line_number());
			}

			this->stack->push(copy_operand(found_op));
			break;
		}

		case op_call_class: {
			unsigned int id = std::stoi(op->operands[0]->identifier);
			int parameter_count = std::stoi(op->operands[1]->identifier);

			CMFunction* code_memory = caller_class->get_cm_class()->member_functions->find(id)->second;

			run_function(vm, caller_class, this, code_memory, parameter_count);

			break;
		}

		case op_cast: {
			Operand* target = this->stack->peek();
			this->stack->pop();
			std::wstring cast_type = op->operands[0]->identifier;

			this->stack->push(cast_operand(op, cast_type, target));

			break;
		}

		case op_call_global: {
			unsigned int id = std::stoi(op->operands[0]->identifier);
			int parameter_count = std::stoi(op->operands[1]->identifier);
			CMFunction* code_memory = vm->global_functions[id];

			run_function(vm, nullptr, this, code_memory, parameter_count);

			break;
		}

		case op_super_call: {
			int parameter_count = std::stoi(op->operands[0]->identifier);
			unsigned int parent_id = caller_class->get_cm_class()->get_parent_id();

			CMClass* parent_code_memory = vm->global_class[parent_id];
			CMFunction* parent_constructor = parent_code_memory->constructor;
			CMFunction* parent_initializer = parent_code_memory->initializer;

			run_function(vm, caller_class, this, parent_constructor, parameter_count);

			run_function(vm, caller_class, this, parent_initializer, 0);

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
			Operand* target_op = this->stack->peek();
			Operand* target = extract_value_of_opernad(target_op);
			this->stack->pop();
			operand_type type = target->get_type();
			Operand* found_op = nullptr;

			if (target->get_type() == operand_null) {
				std::wstring name = op->operands[1]->identifier;

				CHESTNUT_THROW_ERROR(L"Failed to load " + std::wstring(name.begin(), name.end()) + L" because target was null.",
					"RUNTIME_LOAD_FROM_NULL", "0x07", op->get_line_number());
			}

			if (type == operand_address) {
				Memory* memory = reinterpret_cast<Memory*>(std::stoull(target->get_data()));
				found_op = memory->member_variables[std::stoi(op->operands[0]->identifier)];
			}
			else if (type == operand_vector) {
				int index = std::stoi(op->operands[0]->identifier);

				found_op = target->get_vector_elements()->at(index);
			}

			this->stack->push(copy_operand(found_op));

			delete target_op;

			break;
		}

		case op_call_attr: {
			unsigned int id = std::stoi(op->operands[0]->identifier);
			int parameter_count = std::stoi(op->operands[1]->identifier);

			std::vector<Operand*> operands;

			for (int i = 0; i < parameter_count; i++) {
				Operand* op = this->stack->peek();
				this->stack->pop();
				operands.push_back(op);
			}

			Operand* target_op = this->stack->peek();
			Operand* target = extract_value_of_opernad(target_op);
			this->stack->pop();

			for (int i = 0; i < parameter_count; i++) {
				this->stack->push(operands[i]);
			}

			if (target->get_type() == operand_null) {
				std::wstring name = op->operands[2]->identifier;
				CHESTNUT_THROW_ERROR(L"Failed to call " + std::wstring(name.begin(), name.end()) + L" because target was null.",
					"RUNTIME_CALL_FROM_NULL", "0x01", op->get_line_number());
			}

			Memory* memory = reinterpret_cast<Memory*>(std::stoull(target->get_data()));
			CMClass* cm = memory->get_cm_class();
			CMFunction* callee_function = (CMFunction*)cm->member_functions->find(id)->second;

			run_function(vm, memory, this, callee_function, parameter_count);

			delete target_op;

			break;
		}

		}

#ifdef OPERATOR_TIME_STAMP
		finish = clock();

		duration = (double)(finish - start);

		std::wcout << std::endl;
		std::wstring diff = std::to_wstring(duration);
		CHESTNUT_LOG(L"Operator " + std::to_wstring(op->get_type()) + L" end with : " + std::wstring(diff.begin(), diff.end()) + L"ms.", log_level::log_default);
#endif

	}

	vm->stack_area.pop_back();
	delete this;
}