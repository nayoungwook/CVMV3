#include "function_frame.h"

void FunctionFrame::builtin_image(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = op->numeric_operands[1];
	Operand* image = this->stack->peek(); // wstring data.
	this->stack->pop();

	Operand* position = this->stack->peek();
	this->stack->pop();

	std::unordered_map<std::wstring, CMImage*>::iterator image_data_iter
		= vm->resources.find((image)->get_string_data<std::wstring>());

	assert(image_data_iter != vm->resources.end());

	CMImage* image_data = image_data_iter->second;

	Operand* width = nullptr, * height = nullptr;
	width = this->stack->peek(), this->stack->pop();
	height = this->stack->peek(), this->stack->pop();

	std::vector<Operand*>* position_array = (position)->get_vector_elements();

	float _x = position_array->at(0)->get_number_data<float>(),
		_y = position_array->at(1)->get_number_data<float>();

	float f_width = (width)->get_number_data<float>(),
		f_height = (height)->get_number_data<float>();

	Memory* shader_memory = (Memory*)vm->global_area[SHADER_MEMORY]->data;
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	float f_rotation = .0f;

	if (parameter_count == 5) {
		Operand* rotation = this->stack->peek();
		this->stack->pop();

		f_rotation = (rotation)->get_number_data<float>();
		delete rotation;
	}

	render_image(shader_cm, image_data->get_texture(), image_data->get_vao(), _x, _y, f_width, f_height, f_rotation, vm->proj_width, vm->proj_height);

	delete image;
	delete position;
	delete width;
	delete height;
}

void FunctionFrame::builtin_text(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = op->numeric_operands[1];

	Operand* font = this->stack->peek(); // wstring data.
	this->stack->pop();

	Operand* str = this->stack->peek(); // wstring data.
	this->stack->pop();

	Operand* position = this->stack->peek();
	this->stack->pop();

	Operand* size = this->stack->peek();
	this->stack->pop();

	std::vector<Operand*>* position_array = (position)->get_vector_elements();

	float _x = position_array->at(0)->get_number_data<float>(),
		_y = position_array->at(1)->get_number_data<float>();

	float f_rotation = .0f;

	if (parameter_count == 5) {
		Operand* rotation = this->stack->peek();
		this->stack->pop();

		f_rotation = (rotation)->get_number_data<float>();
		delete rotation;
	}

	Memory* shader_memory = (Memory*)(vm->global_area[SHADER_MEMORY]->data);
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	std::wstring font_name = (font)->get_string_data<std::wstring>();
	int i_size = (size)->get_number_data<int>();

	render_text(vm->font_resources[font_name], shader_cm, str->get_string_data<std::wstring>(),
		_x, _y, vm->r, vm->g, vm->b, 1, f_rotation, vm->proj_width, vm->proj_height, i_size);

	delete font;
	delete str;
	delete position;
	delete size;
}


void FunctionFrame::builtin_color(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = op->numeric_operands[1];

	Operand* r = this->stack->peek(); // r
	this->stack->pop();
	Operand* g = this->stack->peek(); // g
	this->stack->pop();
	Operand* b = this->stack->peek(); // b
	this->stack->pop();

	float r_f = r->get_number_data<float>();
	float g_f = g->get_number_data<float>();
	float b_f = b->get_number_data<float>();

	vm->r = (int)(r_f * 255);
	vm->g = (int)(g_f * 255);
	vm->b = (int)(b_f * 255);

	delete r;
	delete g;
	delete b;
}

void FunctionFrame::print_operand(Operand* data) {
	operand_type type = data->get_type();

	switch (type) {
	case operand_number:
		printf("%g", data->get_number_data<double>());
		break;
	case operand_integer:
		printf("%d", data->get_number_data<int>());
		break;
	case operand_float:
		printf("%g", data->get_number_data<float>());
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
		if (memory->get_cm_class()->get_type() == code_member_array) {
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
		std::cout << data->get_bool_data() ? "true" : "false";
		break;
	}

	case operand_string: {
		std::wcout << data->get_string_data<std::wstring>();
		break;
	}

	case operand_null: {
		std::wcout << L"null";
		break;
	}
	}
}

void FunctionFrame::builtin_print(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = op->numeric_operands[1];

	for (int i = 0; i < parameter_count; i++) {
		Operand* _data = this->stack->peek();
		this->stack->pop();

		Operand* data = (_data);

		this->print_operand(data);

		delete _data;
	}
}

void FunctionFrame::builtin_window(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	int parameter_count = op->numeric_operands[1];

	std::wstring title;
	int width = 0, height = 0;

	for (int i = 0; i < parameter_count; i++) {
		Operand* _data = this->stack->peek();
		this->stack->pop();

		Operand* data = (_data);
		operand_type type = data->get_type();

		if (i == 0) title = data->get_string_data<std::wstring>();
		else if (i == 1) width = data->get_number_data<int>();
		else if (i == 2) height = data->get_number_data<int>();

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
	int parameter_count = op->numeric_operands[1];

	for (int i = 0; i < parameter_count; i++) {
		Operand* _target = this->stack->peek();
		this->stack->pop();

		Operand* target = (_target);

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
	int parameter_count = op->numeric_operands[1];

	Operand* r = this->stack->peek(); // r
	this->stack->pop();
	Operand* g = this->stack->peek(); // g
	this->stack->pop();
	Operand* b = this->stack->peek(); // b
	this->stack->pop();

	float r_f = r->get_number_data<float>();
	float g_f = g->get_number_data<float>();
	float b_f = b->get_number_data<float>();

	glClearColor(r_f, g_f, b_f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	delete r;
	delete g;
	delete b;
}

void FunctionFrame::builtin_random(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	std::random_device rd;

	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> dis(0, 10);

	Operand* result = new Operand((int)dis(gen));

	this->stack->push(result);
}

void FunctionFrame::builtin_random_range(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v1 = this->stack->peek();
	this->stack->pop();
	Operand* _v2 = this->stack->peek();
	this->stack->pop();

	Operand* v1 = (_v1);
	Operand* v2 = (_v2);

	std::random_device rd;

	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> dis(v1->get_number_data<int>(), v2->get_number_data<int>());

	Operand* result = new Operand(dis(gen));

	this->stack->push(result);

	delete _v1;
	delete _v2;
}

void FunctionFrame::object_builtin_render(CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	CMObject* caller_object = (CMObject*)caller_class->get_cm_class();

	Memory* shader_memory = ((Memory*)vm->global_area[SHADER_MEMORY]->data);
	CMShader* shader_cm = (CMShader*)shader_memory->get_cm_class();

	Operand* texture_op = caller_class->member_variables[OBJECT_SPRITE];

	if (texture_op->get_type() == operand_null) {
		std::wstring name = caller_class->get_cm_class()->name;
		CHESTNUT_THROW_ERROR(L"Failed to render " + std::wstring(name.begin(), name.end()) + L". You must assing texture for it.",
			"RUNTIME_NO_TEXTURE_FOR_OBJECT", "0x03", 0);
	}

	Operand* position_op = caller_class->member_variables[OBJECT_POSITION];

	float _x = (position_op->get_vector_elements()->at(0))->get_number_data<float>(),
		_y = (position_op->get_vector_elements()->at(1))->get_number_data<float>();

	Operand* width_op = caller_class->member_variables[OBJECT_WIDTH];
	Operand* height_op = caller_class->member_variables[OBJECT_HEIGHT];

	float f_width = (width_op)->get_number_data<float>()
		, f_height = (height_op)->get_number_data<float>();

	Operand* rotation_op = caller_class->member_variables[OBJECT_ROTATION];
	float f_rotation = (rotation_op)->get_number_data<float>();

	std::wstring image_name = (texture_op)->get_string_data<std::wstring>();
	std::unordered_map<std::wstring, CMImage*>::iterator image_data_iter = vm->resources.find(image_name);

	assert(image_data_iter != vm->resources.end());

	CMImage* image_data = image_data_iter->second;

	render_image(shader_cm, image_data->get_texture(), image_data->get_vao(),
		_x, _y, f_width, f_height, f_rotation, vm->proj_width, vm->proj_height);
}

// Builtin Functions for in class
void FunctionFrame::run_member_builtin(code_type cm_type, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	switch (cm_type) {
	case code_member_render: {
		object_builtin_render(vm, caller, caller_class);
		break;
	}

	case code_member_array_push: {
		Operand* target_element = this->stack->peek();
		this->stack->pop();

		((ArrayMemory*)caller_class)->array_elements->push_back(target_element);

		if (target_element->get_type() == operand_address) {
			// modifying nodes for attr
			gc_nodes[caller_class]->childs.push_back(gc_nodes[target_element->get_memory_data()]);
		}
		break;
	}

	case code_member_array_size: {
		int size_wstr = (int)((ArrayMemory*)caller_class)->array_elements->size();
		Operand* result = new Operand(size_wstr);
		caller->stack->push(result);
		break;
	}

	case code_member_array_remove: {

		Operand* target_element = this->stack->peek();
		this->stack->pop();

		std::vector<Operand*>* _array = ((ArrayMemory*)caller_class)->array_elements;

		int index = 0;

		for (Operand* _element : (*_array)) {
			if (compare_operand((_element), target_element)) {
				break;
			}
			index++;
		}

		if (index == _array->size()) { // Failed to find element in array.
			delete this;
			return;
		}

		if (target_element->get_type() == operand_address) {
			disconnectNode(caller_class, (Memory*)target_element);
		}

		_array->erase(_array->begin() + index);

		break;
	}

	case code_member_array_set: {
		Operand* target_element = this->stack->peek();
		this->stack->pop();

		Operand* _index = this->stack->peek();
		this->stack->pop();

		std::vector<Operand*>* _array = ((ArrayMemory*)caller_class)->array_elements;

		int index = _index->get_number_data<int>();

		if (_array->at(index)->get_type() == operand_address) {
			disconnectNode(caller_class, _array->at(index)->get_memory_data());
		}

		_array->at(index) = target_element;

		if (target_element->get_type() == operand_address) {
			// modifying nodes for attr
			gc_nodes[caller_class]->childs.push_back(gc_nodes[target_element->get_memory_data()]);
		}
		break;
	}

	}

	delete this;
	return;
}

void FunctionFrame::run_builtin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	unsigned int id = op->numeric_operands[0];
	int parameter_count = op->numeric_operands[1];

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