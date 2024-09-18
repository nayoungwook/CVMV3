#include "function_frame.h"

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

	std::vector<Operand*>* position_array = extract_value_of_opernad(position)->get_array_data();

	float _x = std::stof(position_array->at(0)->get_data()),
		_y = std::stof(position_array->at(1)->get_data());

	float f_width = std::stof(extract_value_of_opernad(width)->get_data()),
		f_height = std::stof(extract_value_of_opernad(height)->get_data());

	Memory* shader_memory = reinterpret_cast<Memory*>(std::stoull(vm->global_area[SHADER_MEMORY]->get_data()));
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	float f_rotation = .0f;

	if (parameter_count == 5) {
		Operand* rotation = this->stack->peek();
		this->stack->pop();

		f_rotation = std::stof(extract_value_of_opernad(rotation)->get_data());
		delete rotation;
	}

	render_image(shader_cm, image_data->get_texture(), image_data->get_vao(), _x, _y, f_width, f_height, f_rotation, vm->proj_width, vm->proj_height);

	delete image;
	delete position;
	delete width;
	delete height;
}

void FunctionFrame::builtin_text(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->get_operands()[1]->identifier);
	Operand* str = this->stack->peek(); // string data.
	this->stack->pop();

	Operand* position = this->stack->peek();
	this->stack->pop();

	std::vector<Operand*>* position_array = extract_value_of_opernad(position)->get_array_data();

	float _x = std::stof(position_array->at(0)->get_data()),
		_y = std::stof(position_array->at(1)->get_data());

	float f_rotation = .0f;

	if (parameter_count == 5) {
		Operand* rotation = this->stack->peek();
		this->stack->pop();

		f_rotation = std::stof(extract_value_of_opernad(rotation)->get_data());
		delete rotation;
	}

	render_text(vm->renderer, str->get_data(), _x, _y, 1, 1, 1, 1, f_rotation);

	delete str;
	delete position;
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
		for (int i = 0; i < data->get_array_data()->size(); i++) {
			print_operand(data->get_array_data()->at(i));
			if (i != data->get_array_data()->size() - 1) {
				std::cout << ",";
			}
		}
		std::cout << ")";
		break;
	case operand_array:
		std::cout << "[";
		for (int i = 0; i < data->get_array_data()->size(); i++) {
			print_operand(data->get_array_data()->at(i));
			if (i != data->get_array_data()->size() - 1) {
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
	int width = 0, height = 0;

	for (int i = 0; i < parameter_count; i++) {
		Operand* _data = this->stack->peek();
		this->stack->pop();

		Operand* data = extract_value_of_opernad(_data);
		operand_type type = data->get_type();

		if (i == 0) title = data->get_data();
		else if (i == 1) width = (int)std::stod(data->get_data());
		else if (i == 2) height = (int)std::stod(data->get_data());

		vm->proj_width = width;
		vm->proj_height = height;

		delete _data;
	}

	unsigned int builtin_id = vm->builtin_class.size();
	vm->builtin_class.insert(std::make_pair(builtin_id, new CMWindow(builtin_id, vm, title, width, height)));

	Memory* win_memory = new Memory(vm->builtin_class.find(builtin_id));

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

void FunctionFrame::builtin_random(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	std::random_device rd;

	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> dis(0, 999999999);

	this->stack->push(new Operand(std::to_string(dis(gen)), operand_number));
}

void FunctionFrame::builtin_sin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	this->stack->push(new Operand(std::to_string(
		std::sin(std::stof(v->get_data()))), operand_number));

	delete _v;
}

void FunctionFrame::builtin_cos(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	this->stack->push(new Operand(std::to_string(
		std::cos(std::stof(v->get_data()))), operand_number));

	delete _v;
}

void FunctionFrame::builtin_tan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	this->stack->push(new Operand(std::to_string(
		std::tan(std::stof(v->get_data()))), operand_number));

	delete _v;
}

void FunctionFrame::builtin_atan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v1 = this->stack->peek();
	this->stack->pop();
	Operand* _v2 = this->stack->peek();
	this->stack->pop();

	Operand* v1 = extract_value_of_opernad(_v1);
	Operand* v2 = extract_value_of_opernad(_v2);

	this->stack->push(new Operand(std::to_string(
		std::atan2(std::stof(v1->get_data()), std::stof(v2->get_data()))), operand_number));

	delete _v1;
	delete _v2;
}

void FunctionFrame::builtin_abs(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	this->stack->push(new Operand(std::to_string(
		std::abs(std::stof(v->get_data()))), operand_number));

	delete _v;
}

void FunctionFrame::builtin_random_range(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v1 = this->stack->peek();
	this->stack->pop();
	Operand* _v2 = this->stack->peek();
	this->stack->pop();

	Operand* v1 = extract_value_of_opernad(_v1);
	Operand* v2 = extract_value_of_opernad(_v2);

	std::random_device rd;

	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> dis(std::stof(v1->get_data()), std::stof(v2->get_data()));

	this->stack->push(new Operand(std::to_string(dis(gen)), operand_number));

	delete _v1;
	delete _v2;
}

void FunctionFrame::builtin_sqrt(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	this->stack->push(new Operand(std::to_string(
		std::sqrt(std::stof(v->get_data()))), operand_number));

	delete _v;
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
	case BUILTIN_BACKGROUND: // background
		this->builtin_background(op, vm, caller, caller_class);
		break;
	case BUILTIN_RANDOM: // random
		this->builtin_random(op, vm, caller, caller_class);
		break;
	case BUILTIN_SIN: // sin
		this->builtin_sin(op, vm, caller, caller_class);
		break;
	case BUILTIN_COS: // cos
		this->builtin_cos(op, vm, caller, caller_class);
		break;
	case BUILTIN_TAN: // tan
		this->builtin_tan(op, vm, caller, caller_class);
		break;
	case BUILTIN_ATAN: // atan
		this->builtin_atan(op, vm, caller, caller_class);
		break;
	case BUILTIN_ABS: // abs
		this->builtin_abs(op, vm, caller, caller_class);
		break;
	case BUILTIN_RANDOM_RANGE: // random_range
		this->builtin_random_range(op, vm, caller, caller_class);
		break;
	case BUILTIN_SQRT:
		this->builtin_sqrt(op, vm, caller, caller_class);
		break;
	case BUILTIN_TEXT: // text
		this->builtin_text(op, vm, caller, caller_class);
		break;
	}
}