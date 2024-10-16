#include "function_frame.h"

void FunctionFrame::builtin_image(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->operands[1]->identifier);
	Operand* image = this->stack->peek(); // wstring data.
	this->stack->pop();

	Operand* position = this->stack->peek();
	this->stack->pop();

	std::unordered_map<std::wstring, CMImage*>::iterator image_data_iter = vm->resources.find(*((std::wstring*)extract_value_of_opernad(image)->data));

	assert(image_data_iter != vm->resources.end());

	CMImage* image_data = image_data_iter->second;

	Operand* width = nullptr, * height = nullptr;
	width = this->stack->peek(), this->stack->pop();
	height = this->stack->peek(), this->stack->pop();

	std::vector<Operand*>* position_array = extract_value_of_opernad(position)->get_vector_elements();

	float _x = (float)*((double*)position_array->at(0)->data),
		_y = (float)*((double*)position_array->at(1)->data);

	float f_width = (float)*((double*)extract_value_of_opernad(width)->data),
		f_height = (float)*((double*)extract_value_of_opernad(height)->data);

	Memory* shader_memory = (Memory*)vm->global_area[SHADER_MEMORY]->data;
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	float f_rotation = .0f;

	if (parameter_count == 5) {
		Operand* rotation = this->stack->peek();
		this->stack->pop();

		f_rotation = (float)*((double*)extract_value_of_opernad(rotation)->data);
		delete rotation;
	}

	render_image(shader_cm, image_data->get_texture(), image_data->get_vao(), _x, _y, f_width, f_height, f_rotation, vm->proj_width, vm->proj_height);

	delete image;
	delete position;
	delete width;
	delete height;
}

void FunctionFrame::builtin_text(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->operands[1]->identifier);

	Operand* font = this->stack->peek(); // wstring data.
	this->stack->pop();

	Operand* str = this->stack->peek(); // wstring data.
	this->stack->pop();

	Operand* position = this->stack->peek();
	this->stack->pop();

	Operand* size = this->stack->peek();
	this->stack->pop();

	std::vector<Operand*>* position_array = extract_value_of_opernad(position)->get_vector_elements();

	float _x = (float)*((double*)position_array->at(0)->data),
		_y = (float)*((double*)position_array->at(1)->data);

	float f_rotation = .0f;

	if (parameter_count == 5) {
		Operand* rotation = this->stack->peek();
		this->stack->pop();

		f_rotation = (float)*((double*)extract_value_of_opernad(rotation)->data);
		delete rotation;
	}

	Memory* shader_memory = (Memory*)(vm->global_area[SHADER_MEMORY]->data);
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	std::wstring font_name = *((std::wstring*)extract_value_of_opernad(font)->data);
	int i_size = (int)*((double*)extract_value_of_opernad(size)->data);

	render_text(vm->font_resources[font_name], shader_cm, *((std::wstring*)str->data),
		_x, _y, vm->r, vm->g, vm->b, 1, f_rotation, vm->proj_width, vm->proj_height, i_size);

	delete font;
	delete str;
	delete position;
	delete size;
}


void FunctionFrame::builtin_color(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->operands[1]->identifier);

	Operand* _r = this->stack->peek(); // r
	this->stack->pop();
	Operand* _g = this->stack->peek(); // g
	this->stack->pop();
	Operand* _b = this->stack->peek(); // b
	this->stack->pop();

	Operand* r = extract_value_of_opernad(_r),
		* g = extract_value_of_opernad(_g),
		* b = extract_value_of_opernad(_b);

	float r_f = (float)*((double*)r->data);
	float g_f = (float)*((double*)g->data);
	float b_f = (float)*((double*)b->data);

	vm->r = (int)(r_f * 255);
	vm->g = (int)(g_f * 255);
	vm->b = (int)(b_f * 255);

	delete _r;
	delete _g;
	delete _b;
}

void FunctionFrame::print_operand(Operand* data) {
	operand_type type = data->get_type();

	switch (type) {
	case operand_number:
		printf("%g", *((double*)data->data));
		break;
	case operand_vector:
		std::cout << "(";
		for (int i = 0; i < data->get_vector_elements()->size(); i++) {
			print_operand(data->get_vector_elements()->at(i));
			if (i != data->get_vector_elements()->size() - 1) {
				std::cout << ",";
			}
		}
		std::cout << ")";
		break;

	case operand_address: {
		Memory* memory = (Memory*)(data->data);
		if (memory->get_cm_class()->get_type() == code_array) {
			ArrayMemory* arr_memory = (ArrayMemory*)memory;
			std::cout << "[";
			for (int i = 0; i < arr_memory->array_elements->size(); i++) {
				print_operand(arr_memory->array_elements->at(i));
				if (i != arr_memory->array_elements->size() - 1) {
					std::cout << ",";
				}
			}
			std::cout << "]";
		}
		else {
			std::wcout << memory->get_cm_class()->name;
		}

		break;
	}
	case operand_bool: {
		std::cout << *((bool*)data->data) ? "true" : "false";
		break;
	}

	case operand_string: {
		std::wcout << *((std::wstring*)data->data);
		break;
	}

	case operand_null: {
		std::wcout << L"null";
		break;
	}
	}
}

void FunctionFrame::builtin_print(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->operands[1]->identifier);

	for (int i = 0; i < parameter_count; i++) {
		Operand* _data = this->stack->peek();
		this->stack->pop();

		Operand* data = extract_value_of_opernad(_data);

		this->print_operand(data);

		delete _data;
	}
}

void FunctionFrame::builtin_window(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->operands[1]->identifier);

	std::wstring title;
	int width = 0, height = 0;

	for (int i = 0; i < parameter_count; i++) {
		Operand* _data = this->stack->peek();
		this->stack->pop();

		Operand* data = extract_value_of_opernad(_data);
		operand_type type = data->get_type();

		if (i == 0) title = *((std::wstring*)data->data);
		else if (i == 1) width = (int)*((double*)data->data);
		else if (i == 2) height = (int)*((double*)data->data);

		vm->proj_width = width;
		vm->proj_height = height;

		delete _data;
	}

	unsigned int builtin_id = vm->builtin_class.size();
	vm->builtin_class.insert(std::make_pair(builtin_id, new CMWindow(builtin_id, vm, title, width, height)));

	Memory* win_memory = new Memory(vm->builtin_class.find(builtin_id));

	this->stack->push(create_address_operand(win_memory));
}

extern std::unordered_map<Memory*, Node*> gc_nodes;

void FunctionFrame::builtin_load_scene(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->operands[1]->identifier);

	for (int i = 0; i < parameter_count; i++) {
		Operand* _target = this->stack->peek();
		this->stack->pop();

		Operand* target = extract_value_of_opernad(_target);

		operand_type type = target->get_type();
		Memory* scene = (Memory*)target->data;

		vm->current_scene_memory = scene;
		vm->gc->current_scene = scene;

		CMFunction* init_function =
			scene->get_cm_class()->member_functions->find(scene->get_cm_class()->get_init_function_id())->second;
		run_function(vm, vm->current_scene_memory, nullptr, init_function, 0);

		delete _target;
	}
}

void FunctionFrame::builtin_background(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = std::stoi(op->operands[1]->identifier);

	Operand* _r = this->stack->peek(); // r
	this->stack->pop();
	Operand* _g = this->stack->peek(); // g
	this->stack->pop();
	Operand* _b = this->stack->peek(); // b
	this->stack->pop();

	Operand* r = extract_value_of_opernad(_r),
		* g = extract_value_of_opernad(_g),
		* b = extract_value_of_opernad(_b);

	float r_f = (float)*((double*)r->data);
	float g_f = (float)*((double*)g->data);
	float b_f = (float)*((double*)b->data);

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

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = dis(gen);

	this->stack->push(result);
}

void FunctionFrame::builtin_sin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = std::sin(*((double*)v->data));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::builtin_cos(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = std::cos(*((double*)v->data));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::builtin_tan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = std::tan(*((double*)v->data));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::builtin_atan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v1 = this->stack->peek();
	this->stack->pop();
	Operand* _v2 = this->stack->peek();
	this->stack->pop();

	Operand* v1 = extract_value_of_opernad(_v1);
	Operand* v2 = extract_value_of_opernad(_v2);

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = std::atan2(*((double*)v1->data), *((double*)v2->data));

	this->stack->push(result);

	delete _v1;
	delete _v2;
}

void FunctionFrame::builtin_abs(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = std::sin(*((double*)v->data));

	this->stack->push(result);

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

	std::uniform_int_distribution<int> dis((float)*((double*)v1->data), (float)*((double*)v2->data));

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = dis(gen);

	this->stack->push(result);

	delete _v1;
	delete _v2;
}

void FunctionFrame::builtin_sqrt(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = extract_value_of_opernad(_v);

	Operand* result = new Operand(8, operand_number);
	*((double*)result->data) = std::sqrt(*((double*)v->data));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::run_builtin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	unsigned int id = std::stoi(op->operands[0]->identifier);
	int parameter_count = std::stoi(op->operands[1]->identifier);

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
	case BUILTIN_COLOR: // COLOR
		this->builtin_color(op, vm, caller, caller_class);
		break;
	}
}