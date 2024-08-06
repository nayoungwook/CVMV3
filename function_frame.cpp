#include "function_frame.h"

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

Operand* copy_operand(Operand* op) {
	Operand* copied_op = nullptr;

	op = extract_value_of_opernad(op);

	if (op->get_type() == operand_vector || op->get_type() == operand_array) {
		copied_op = new Operand(op->get_array_data(), op->get_type());
	}
	else {
		copied_op = new Operand(op->get_data(), op->get_type());
	}

	return copied_op;
}

inline Operand* extract_value_of_opernad(Operand* op) {

	Operand* result = op;

	if (op->get_type() == operand_op_address) {
		while (result->get_type() == operand_op_address) {
			result = reinterpret_cast<Operand*>(std::stoull(op->get_data()));
		}
	}

	return result;
}

Operand* create_address_operand(Memory* op) {
	return new Operand(std::to_string((unsigned long long)(void**) op), operand_address);
}

Operand* create_op_address_operand(Operand* op) {
	return new Operand(std::to_string((unsigned long long)(void**) op), operand_op_address);
}

const std::string get_type_string_of_operand(Operand* op) {
	std::string type_string_operand = "";

	Operand* extracted_op = extract_value_of_opernad(op);

	switch (extracted_op->get_type()) {
	case operand_number:
		type_string_operand = "number";
		break;
	case operand_array:
		type_string_operand = "array";
		break;
	case operand_address:
		type_string_operand
			= reinterpret_cast<Memory*>(std::stoull(extracted_op->get_data()))->get_cm_class()->name;
		break;
	case operand_bool:
		type_string_operand = "bool";
		break;
	case operand_string:
		type_string_operand = "string";
		break;
	case operand_vector:
		type_string_operand = "vector";
		break;
	}

	return type_string_operand;
}

void run_function(CVM* vm, Memory* caller_class, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count) {
	FunctionFrame* frame = new FunctionFrame(code_memory);

	if (parameter_count != code_memory->get_param_types().size()) {
		std::cout << "Error with function parameter : " << code_memory->name << std::endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < parameter_count; i++) {
		Operand* op = caller_frame->stack->peek();
		caller_frame->stack->pop();

		std::string type_string_of_operand = get_type_string_of_operand(op);

		if (code_memory->get_param_types()[i] != type_string_of_operand) {
			//			std::wstring w_function_name;
			//			w_function_name.assign(code_memory->name.begin(), code_memory->name.end());

			//			CHESTNUT_THROW_ERROR(L"We don\'t have function \'" + w_function_name + L"\' with this parameter.", "NO_FUNCTION_WITH_PARAMETER", "008", ast->line_number);

			std::cout << "Error with function parameter : " << code_memory->name << std::endl;
			exit(EXIT_FAILURE);
		}

		frame->local_area.insert(std::make_pair(i, copy_operand(op)));
	}

	frame->run(vm, caller_frame, caller_class);
}

void FunctionFrame::builtin_image(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->get_operands()[1]->identifier);

	Operand* image = this->stack->peek(); // string data.
	this->stack->pop();

	Operand* position = this->stack->peek();
	this->stack->pop();

	std::unordered_map<std::string, CMImage*>::iterator image_data_iter = vm->resources.find(extract_value_of_opernad(image)->get_data());

	assert(image_data_iter != vm->resources.end());

	CMImage* image_data = image_data_iter->second;

	Operand* width = nullptr, * height = nullptr;
	width = this->stack->peek(), this->stack->pop();
	height = this->stack->peek(), this->stack->pop();

	float _x = std::stof(extract_value_of_opernad(position)->get_array_data()[0]->get_data()),
		_y = std::stof(extract_value_of_opernad(position)->get_array_data()[1]->get_data());

	float f_width = std::stof(extract_value_of_opernad(width)->get_data()), f_height = std::stof(extract_value_of_opernad(height)->get_data());

	Memory* shader_memory = reinterpret_cast<Memory*>(std::stoull(vm->global_area[SHADER_MEMORY]->get_data()));
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	float f_rotation = .0f;

	if (parameter_count == 5) {
		Operand* rotation = this->stack->peek();
		this->stack->pop();

		f_rotation = std::stof(extract_value_of_opernad(rotation)->get_data());
	}

	render_image(shader_cm, image_data->get_texture(), image_data->get_vao(), _x, _y, f_width, f_height, f_rotation);

	delete image;
	delete position;
	delete width;
	delete height;
}

void FunctionFrame::print_operand(Operand* data) {
	operand_type type = data->get_type();
	std::string content = data->get_data();

	switch (type) {
	case operand_number:
		printf("%g", std::stod(content));
		break;
	case operand_vector:
		std::cout << "(";
		for (int i = 0; i < data->get_array_data().size(); i++) {
			print_operand(data->get_array_data()[i]);
			if (i != data->get_array_data().size() - 1) {
				std::cout << ",";
			}
		}
		std::cout << ")";
		break;
	case operand_array:
		std::cout << "[";
		for (int i = 0; i < data->get_array_data().size(); i++) {
			print_operand(data->get_array_data()[i]);
			if (i != data->get_array_data().size() - 1) {
				std::cout << ",";
			}
		}
		std::cout << "]";
		break;
	default:
		std::cout << content;
		break;
	}
}

void FunctionFrame::builtin_print(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->get_operands()[1]->identifier);

	for (int i = 0; i < parameter_count; i++) {
		Operand* _data = this->stack->peek();
		this->stack->pop();

		Operand* data = extract_value_of_opernad(_data);

		this->print_operand(data);

		delete _data;
	}
}

void FunctionFrame::builtin_window(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->get_operands()[1]->identifier);

	std::string title;
	int width, height;

	for (int i = 0; i < parameter_count; i++) {
		Operand* _data = this->stack->peek();
		this->stack->pop();

		Operand* data = extract_value_of_opernad(_data);
		operand_type type = data->get_type();

		if (i == 0) title = data->get_data();
		else if (i == 1) width = (int)std::stod(data->get_data());
		else if (i == 2) height = (int)std::stod(data->get_data());

		delete _data;
	}

	CMWindow* cm_window = new CMWindow(vm, title, width, height);
	Memory* win_memory = new Memory(cm_window);

	this->stack->push(create_address_operand(win_memory));
}

void FunctionFrame::builtin_load_scene(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->get_operands()[1]->identifier);

	for (int i = 0; i < parameter_count; i++) {
		Operand* _target = this->stack->peek();
		this->stack->pop();

		Operand* target = extract_value_of_opernad(_target);

		operand_type type = target->get_type();
		Memory* scene = reinterpret_cast<Memory*>(std::stoull(target->get_data()));

		vm->current_scene_memory = scene;

		CMFunction* init_function =
			scene->get_cm_class()->member_functions->find(scene->get_cm_class()->get_init_function_id())->second;
		run_function(vm, vm->current_scene_memory, nullptr, init_function, 0);

		delete _target;
	}
}

void FunctionFrame::builtin_background(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->get_operands()[1]->identifier);

	Operand* _r = this->stack->peek(); // r
	this->stack->pop();
	Operand* _g = this->stack->peek(); // g
	this->stack->pop();
	Operand* _b = this->stack->peek(); // b
	this->stack->pop();

	Operand* r = extract_value_of_opernad(_r),
		* g = extract_value_of_opernad(_g),
		* b = extract_value_of_opernad(_b);

	float r_f = std::stof(r->get_data());
	float g_f = std::stof(g->get_data());
	float b_f = std::stof(b->get_data());

	glClearColor(r_f, g_f, b_f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	delete _r;
	delete _g;
	delete _b;
}

void FunctionFrame::run_builtin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	unsigned int id = std::stoi(op->get_operands()[0]->identifier);
	int parameter_count = std::stoi(op->get_operands()[1]->identifier);

	switch (id) {
	case BUILTIN_PRINT: // print
		this->builtin_print(op, vm, caller, caller_class);
		break;
	case BUILTIN_WINDOW:// window
		this->builtin_window(op, vm, caller, caller_class);
		break;
	case BUILTIN_LOAD_SCENE: // load_scene
		this->builtin_load_scene(op, vm, caller, caller_class);
		break;
	case BUILTIN_IMAGE: // image
		this->builtin_image(op, vm, caller, caller_class);
		break;
	case BUILTIN_BACKGROUND: // image
		this->builtin_background(op, vm, caller, caller_class);
		break;
	}
}

Operand* calcaulte_vector_operand(Operand* lhs, Operand* rhs, double (*cal)(double l, double r)) {
	Operand* lhs_vector = extract_value_of_opernad(lhs);
	Operand* rhs_vector = extract_value_of_opernad(rhs);

	std::vector<Operand*> calculated_result;

	for (int i = 0; i < (int)min(lhs_vector->get_array_data().size(), rhs_vector->get_array_data().size()); i++) {
		double lhs_v = std::stod(lhs_vector->get_array_data()[i]->get_data());
		double rhs_v = std::stod(rhs_vector->get_array_data()[i]->get_data());

		double calculated_v = cal(lhs_v, rhs_v);

		calculated_result.push_back(new Operand(std::to_string(calculated_v), operand_number));
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

Memory* create_object(CVM* vm, CMClass* code_memory, FunctionFrame* frame, unsigned int constructor_parameter_count) {

	Memory* memory = new Memory(code_memory);

	// run initializer
	run_function(vm, memory, frame, code_memory->initializer, 0);

	// run constructor
	run_function(vm, memory, frame, code_memory->constructor, constructor_parameter_count);

	if (code_memory->get_type() == code_object) {

		// add render function
		unsigned int render_function_id = ((CMObject*)code_memory)->get_render_function_id();
		std::vector<Operator*> temp1;
		std::vector<std::string> temp2;
		CMFunction* render_function = new CMRender(temp1, render_function_id, temp2);
		if (code_memory->member_functions->find(render_function_id) != code_memory->member_functions->end())
			code_memory->member_functions->erase(code_memory->member_functions->find(render_function_id));

		code_memory->member_functions->insert(std::make_pair(render_function_id, render_function));

		// add primitive variables
		bool position_declared = memory->member_variables.find(OBJECT_POSITION) != memory->member_variables.end();
		bool width_declared = memory->member_variables.find(OBJECT_WIDTH) != memory->member_variables.end();
		bool height_declared = memory->member_variables.find(OBJECT_HEIGHT) != memory->member_variables.end();
		bool rotation_declared = memory->member_variables.find(OBJECT_ROTATION) != memory->member_variables.end();
		bool texture_declared = memory->member_variables.find(OBJECT_SPRITE) != memory->member_variables.end();

		if (!position_declared) {
			std::vector<Operand*> position_data;
			position_data.push_back(new Operand("0", operand_number));
			position_data.push_back(new Operand("0", operand_number));
			memory->member_variables.insert(std::make_pair(OBJECT_POSITION, new Operand(position_data, operand_vector)));
		}

		if (!width_declared) memory->member_variables.insert(std::make_pair(OBJECT_WIDTH, new Operand("100", operand_number)));
		if (!height_declared) memory->member_variables.insert(std::make_pair(OBJECT_HEIGHT, new Operand("100", operand_number)));
		if (!rotation_declared) memory->member_variables.insert(std::make_pair(OBJECT_ROTATION, new Operand("0", operand_number)));
		if (!texture_declared) memory->member_variables.insert(std::make_pair(OBJECT_SPRITE, new Operand("", operand_null)));

	}
	return memory;
}

bool operand_compare(Operand* op1, Operand* op2) {

	if (op1->get_type() != op2->get_type()) return false;

	if (op1->get_type() == operand_address) {
		return
			reinterpret_cast<Memory*>(std::stoull(op1->get_data())) ==
			reinterpret_cast<Memory*>(std::stoull(op2->get_data()));
	}

	return op1 == op2;
}

void FunctionFrame::run(CVM* vm, FunctionFrame* caller, Memory* caller_class) {

	if (this->get_code_memory()->get_type() == code_render) {
		CMObject* caller_object = (CMObject*)caller_class->get_cm_class();

		Memory* shader_memory = reinterpret_cast<Memory*>(std::stoull(vm->global_area[SHADER_MEMORY]->get_data()));
		CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

		Operand* texture_op = caller_class->member_variables[OBJECT_SPRITE];

		if (texture_op->get_type() == operand_null) {
			std::cout << "error at function_frame\.cpp Debug ID : 01x | texture is not texture memory." << std::endl;
			exit(EXIT_FAILURE);
		}

		Operand* position_op = caller_class->member_variables[OBJECT_POSITION];

		float _x = std::stof(extract_value_of_opernad(position_op->get_array_data()[0])->get_data()),
			_y = std::stof(extract_value_of_opernad(position_op->get_array_data()[1])->get_data());

		Operand* width_op = caller_class->member_variables[OBJECT_WIDTH];
		Operand* height_op = caller_class->member_variables[OBJECT_HEIGHT];

		float f_width = std::stof(extract_value_of_opernad(width_op)->get_data())
			, f_height = std::stof(extract_value_of_opernad(height_op)->get_data());

		Operand* rotation_op = caller_class->member_variables[OBJECT_ROTATION];
		float f_rotation = std::stof(extract_value_of_opernad(rotation_op)->get_data());

		std::unordered_map<std::string, CMImage*>::iterator image_data_iter = vm->resources.find(extract_value_of_opernad(texture_op)->get_data());

		assert(image_data_iter != vm->resources.end());

		CMImage* image_data = image_data_iter->second;

		return 	render_image(shader_cm, image_data->get_texture(), image_data->get_vao(),
			_x, _y, f_width, f_height, f_rotation);
	}

	std::vector<Operator*> operators = this->code_memory->get_operators();

	for (int line = 0; line < operators.size(); line++) {
		Operator* op = operators[line];
		operator_type type = op->get_type();

		switch (type) {

		case op_push_string: {
			std::string data = op->get_operands()[0]->identifier;
			data = data.substr(1, data.size() - 2);

			this->stack->push(new Operand(data, operand_string));
			break;
		}

		case op_push_number: {
			this->stack->push(new Operand(op->get_operands()[0]->identifier, operand_number));
			break;
		}

		case op_push_null: {
			this->stack->push(new Operand("", operand_null));
			break;
		}

		case op_push_this: {
			this->stack->push(create_address_operand(caller_class));
			break;
		}

		case op_push_bool: {
			this->stack->push(new Operand(op->get_operands()[0]->identifier, operand_bool));
			break;
		}

		case op_ret: {
			Operand* op = this->stack->peek();
			this->stack->pop();

			caller->stack->push(op);

			delete this;
			return;
		};

		case op_label: {
			break;
		}

		case op_for: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			if (extract_value_of_opernad(condition)->get_data() == "true") {
				std::string id = op->get_operands()[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			delete condition;

			break;
		}

		case op_new: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);

			CMClass* code_memory = vm->global_class[id];

			Memory* memory = create_object(vm, code_memory, this, parameter_count);

			// store in heap
			vm->heap_area.push_back(memory);

			this->stack->push(create_address_operand(memory));

			break;
		}

		case op_keybaord: {
			std::string key = to_upper_all(op->get_operands()[0]->identifier);
			std::string result = vm->key_data.find(key) != vm->key_data.end() ? "true" : "false";

			this->stack->push(new Operand(result, operand_bool));
			break;
		}

		case op_if: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			if (extract_value_of_opernad(condition)->get_data() == "false") {
				std::string id = op->get_operands()[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			delete condition;

			break;
		}

		case op_inc: {
			Operand* target = this->stack->peek();
			this->stack->pop();

			double v = std::stod(extract_value_of_opernad(target)->get_data()) + 1;
			extract_value_of_opernad(target)->set_data(std::to_string(v));

			delete target;
			break;
		}

		case op_dec: {
			Operand* target = this->stack->peek();
			this->stack->pop();

			double v = std::stod(extract_value_of_opernad(target)->get_data()) - 1;
			extract_value_of_opernad(target)->set_data(std::to_string(v));

			delete target;
			break;
		}

		case op_goto: {
			std::string id = op->get_operands()[0]->identifier;
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

			switch (type) {
			case op_add:
				if (lhs->get_type() == operand_number)
					this->stack->push(new Operand(std::to_string(
						std::stod(lhs->get_data())
						+ std::stod(rhs->get_data())), operand_number));
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_add));

				break;
			case op_sub:
				if (lhs->get_type() == operand_number)
					this->stack->push(new Operand(std::to_string(
						std::stod(lhs->get_data())
						- std::stod(rhs->get_data())), operand_number));
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_sub));

				break;
			case op_mul:
				if (lhs->get_type() == operand_number)
					this->stack->push(new Operand(std::to_string(
						std::stod(lhs->get_data())
						* std::stod(rhs->get_data())), operand_number));
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_mult));

				break;
			case op_div:
				if (lhs->get_type() == operand_number) {
					if (std::stod(rhs->get_data()) == 0) {
						std::cout << "divided by zero error.";
						exit(EXIT_FAILURE);
					}

					this->stack->push(new Operand(std::to_string(
						std::stod(lhs->get_data())
						/ std::stod(rhs->get_data())), operand_number));

				}
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_div));

				break;
			case op_pow:
				if (lhs->get_type() == operand_number) {
					this->stack->push(new Operand(std::to_string(
						pow(std::stod(lhs->get_data()), std::stod(rhs->get_data()))), operand_number));
				}
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_pow));
				break;
			case op_mod: {
				if (lhs->get_type() == operand_number) {
					if (rhs == 0) {
						std::cout << "divided by zero error.";
						exit(EXIT_FAILURE);
					}
					this->stack->push(new Operand(std::to_string(
						(int)std::stod(lhs->get_data())
						% (int)std::stod(rhs->get_data())), operand_number));
				}
				else if (lhs->get_type() == operand_vector)
					this->stack->push(calcaulte_vector_operand(lhs, rhs, cal_mod));
				break;
			}

			case op_greater: {
				bool result = std::stod(lhs->get_data()) < std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_lesser: {
				bool result = std::stod(lhs->get_data()) > std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_eq_lesser: {
				bool result = std::stod(lhs->get_data()) >= std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_eq_greater: {
				bool result = std::stod(lhs->get_data()) <= std::stod(rhs->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_equal: {
				bool result = lhs->get_data() == rhs->get_data();

				if (lhs->get_type() == operand_number)
					result = std::stod(lhs->get_data()) == std::stod(rhs->get_data());

				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_not_equal: {
				bool result = lhs->get_data() != rhs->get_data();
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_or: {
				bool _lhs = lhs->get_data() == "true";
				bool _rhs = rhs->get_data() == "true";
				bool result = _lhs || _rhs;

				this->stack->push(new Operand(result ? "true" : "false", operand_bool));

				break;
			}

			case op_and: {
				bool _lhs = lhs->get_data() == "true";
				bool _rhs = rhs->get_data() == "true";
				bool result = _lhs && _rhs;

				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}
			}
			delete lhs_op;
			delete rhs_op;
			break;
		}

		case op_store_global: {
			Operand* peek = copy_operand(extract_value_of_opernad(this->stack->peek()));
			this->stack->pop();
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);

			if (vm->global_area.find(id) != vm->global_area.end())
				vm->global_area[id] = peek;
			else
				vm->global_area.insert(std::make_pair(id, peek));

			break;
		}

		case op_store_class: {
			Operand* peek = copy_operand(extract_value_of_opernad(this->stack->peek()));
			this->stack->pop();
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);

			if (caller_class->member_variables.find(id) == caller_class->member_variables.end()) {
				//std::cout << "new variable declared." << op->get_operands()[0]->identifier << std::endl;
				caller_class->member_variables.insert(std::make_pair(id, peek));
			}

			break;
		}

		case op_store_local: {
			Operand* peek = copy_operand(extract_value_of_opernad(this->stack->peek()));
			this->stack->pop();
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);

			if (local_area.find(id) != local_area.end())
				local_area[id] = peek;
			else
				local_area.insert(std::make_pair(id, peek));

			break;
		}

		case op_store_attr: {
			Operand* attr_target = extract_value_of_opernad(this->stack->peek());
			this->stack->pop();

			Operand* store_value = copy_operand(extract_value_of_opernad(this->stack->peek()));
			this->stack->pop();

			unsigned int store_id = std::stoi(op->get_operands()[0]->identifier);

			operand_type op_type = attr_target->get_type();

			if (op_type == operand_address) {
				Memory* attr_memory = reinterpret_cast<Memory*>(std::stoull(attr_target->get_data()));

				if (attr_memory->member_variables.find(store_id) != attr_memory->member_variables.end())
					attr_memory->member_variables.erase(attr_memory->member_variables.find(store_id));

				attr_memory->member_variables.insert(std::make_pair(store_id, store_value));
			}
			else if (op_type == operand_vector) {
				Operand* vector_op = attr_target;

				vector_op->get_array_data()[store_id] = store_value;
			}

			break;
		}

		case op_array_get: {
			Operand* index_peek = this->stack->peek();
			this->stack->pop();
			int index = std::stoi(extract_value_of_opernad(index_peek)->get_data());

			Operand* array_operand = this->stack->peek(); // do not delete it.
			this->stack->pop();

			Operand* result = extract_value_of_opernad(array_operand)->get_array_data()[index];

			this->stack->push(copy_operand(result));

			delete index_peek;

			break;
		}

		case op_array: {
			int array_size = std::stoi(op->get_operands()[0]->identifier);

			Operand* result = nullptr;
			std::vector<Operand*> elements;

			for (int i = 0; i < array_size; i++) {
				Operand* peek = this->stack->peek();
				this->stack->pop();

				elements.push_back(peek);
			}

			result = new Operand(elements, operand_array);

			this->stack->push(result);

			break;
		}

		case op_vector: {
			int vector_size = std::stoi(op->get_operands()[0]->identifier);

			Operand* result = nullptr;
			std::vector<Operand*> elements;

			for (int i = 0; i < vector_size; i++) {
				Operand* _peek = this->stack->peek();
				Operand* extracted_data = extract_value_of_opernad(_peek);
				this->stack->pop();

				elements.push_back(extracted_data);
			}

			result = new Operand(elements, operand_vector);

			this->stack->push(result);

			break;
		}

		case op_load_global: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			Operand* found_op = vm->global_area[id];

			this->stack->push(create_op_address_operand(found_op));

			break;
		}

		case op_load_local: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			Operand* found_op = local_area[id];

			this->stack->push(create_op_address_operand(found_op));
			break;
		}

		case op_load_class: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			Operand* found_op = caller_class->member_variables[id];

			this->stack->push(create_op_address_operand(found_op));
			break;
		}

		case op_call_class: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);

			CMFunction* code_memory = caller_class->get_cm_class()->member_functions->find(id)->second;

			run_function(vm, nullptr, this, code_memory, parameter_count);

			break;
		}

		case op_call_global: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);
			CMFunction* code_memory = vm->global_functions[id];

			run_function(vm, nullptr, this, code_memory, parameter_count);

			break;
		}

		case op_super_call: {
			int parameter_count = std::stoi(op->get_operands()[0]->identifier);
			unsigned int parent_id = caller_class->get_cm_class()->get_parent_id();

			CMClass* parent_code_memory = vm->global_class[parent_id];
			CMFunction* parent_constructor = parent_code_memory->constructor;
			CMFunction* parent_initializer = parent_code_memory->initializer;

			run_function(vm, caller_class, this, parent_constructor, parameter_count);

			run_function(vm, caller_class, this, parent_initializer, 0);

			std::unordered_map<unsigned int, CMFunction*>::iterator member_function_iter = parent_code_memory->member_functions->begin();

			for (; member_function_iter != parent_code_memory->member_functions->end(); member_function_iter++) {
				if (member_function_iter->second->get_access_modifier() != "private")
					caller_class->get_cm_class()->member_functions->insert(std::make_pair(member_function_iter->second->get_id(), member_function_iter->second));
			}

			break;
		}

		case op_call_builtin: {
			this->run_builtin(op, vm, this, caller_class);
			break;
		}

		case op_load_attr: {
			Operand* target = extract_value_of_opernad(this->stack->peek());
			this->stack->pop();
			operand_type type = target->get_type();
			Operand* found_op = nullptr;

			if (type == operand_address) {
				Memory* memory = reinterpret_cast<Memory*>(std::stoull(target->get_data()));
				found_op = memory->member_variables[std::stoi(op->get_operands()[0]->identifier)];
			}
			else if (type == operand_vector) {
				int index = std::stoi(op->get_operands()[0]->identifier);

				found_op = target->get_array_data()[index];
			}

			this->stack->push(create_op_address_operand(found_op));

			break;
		}

		case op_call_attr: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);

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
				std::cout << "error at function_frame\.cpp op_call_attr | Error Code : WWERS" << std::endl;
				exit(EXIT_FAILURE);
			}

			if (target->get_type() == operand_array) {
				if (id == 0) { // array.push( e )
					Operand* _target_element = this->stack->peek();
					this->stack->pop();

					Operand* target_element = extract_value_of_opernad(_target_element);

					target->get_array_data().push_back(target_element);
				}
				else if (id == 1) { // array.size()
					Operand* result = new Operand(std::to_string(target->get_array_data().size()), operand_number);

					this->stack->push(result);
				}
				else if (id == 2) { // array.remove( e )
					Operand* _target_element = this->stack->peek();
					this->stack->pop();

					Operand* target_element = extract_value_of_opernad(_target_element);
					std::vector<Operand*>* _array = &target->get_array_data();

					int index = 0;

					for (Operand* _element : (*_array)) {
						if (operand_compare(extract_value_of_opernad(_element), target_element)) {
							break;
						}
						index++;
					}

					if (index == _array->size()) { // Failed to find element in array.

					}

					_array->erase(_array->begin() + index);
				}
			}
			else {
				Memory* memory = reinterpret_cast<Memory*>(std::stoull(target->get_data()));
				CMClass* cm = memory->get_cm_class();
				CMFunction* callee_function = (CMFunction*)cm->member_functions->find(id)->second;

				run_function(vm, memory, this, callee_function, parameter_count);
			}

			delete target_op;

			break;
		}

		}
	}
	delete this;
}