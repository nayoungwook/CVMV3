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

	if (op->get_type() == operand_vector || op->get_type() == operand_array) {
		std::vector<Operand*> array_data = op->get_array_data();
		copied_op = new Operand(array_data, op->get_type());
	}
	else {
		copied_op = new Operand(op->get_data(), op->get_type());
	}

	return copied_op;
}

inline Operand* extract_value_of_opernad(Operand* op) {

	Operand* result = op;

	if (op->get_type() == operand_op_address) {
		result = reinterpret_cast<Operand*>(std::stoull(op->get_data()));
	}

	return result;
}

Operand* create_address_operand(Memory* op) {
	return new Operand(std::to_string((unsigned long long)(void**) op), operand_address);
}

Operand* create_op_address_operand(Operand* op) {
	return new Operand(std::to_string((unsigned long long)(void**) op), operand_op_address);
}

void run_function(CVM* vm, Memory* caller_class, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count) {
	FunctionFrame* frame = new FunctionFrame(code_memory);

	for (int i = 0; i < parameter_count; i++) {
		Operand* op = caller_frame->stack->peek();
		caller_frame->stack->pop();


		frame->local_area.insert(std::make_pair(i, copy_operand(op)));
	}

	frame->run(vm, caller_frame, caller_class);
}

void FunctionFrame::run(CVM* vm, FunctionFrame* caller, Memory* caller_class) {
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

			Memory* memory = new Memory(code_memory);

			// run constructor
			run_function(vm, memory, this, code_memory->constructor, parameter_count);

			// run initializer
			run_function(vm, memory, this, code_memory->initializer, 0);

			// store in heap
			vm->heap_area.push_back(memory);

			this->stack->push(create_address_operand(memory));

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
			Operand* lhs = this->stack->peek();
			this->stack->pop();
			Operand* rhs = this->stack->peek();
			this->stack->pop();

			switch (type) {
			case op_add:
				this->stack->push(new Operand(std::to_string(
					std::stod(extract_value_of_opernad(lhs)->get_data())
					+ std::stod(extract_value_of_opernad(rhs)->get_data())), operand_number));
				break;
			case op_sub:
				this->stack->push(new Operand(std::to_string(
					std::stod(extract_value_of_opernad(lhs)->get_data())
					- std::stod(extract_value_of_opernad(rhs)->get_data())), operand_number));
				break;
			case op_mul:
				this->stack->push(new Operand(std::to_string(
					std::stod(extract_value_of_opernad(lhs)->get_data())
					* std::stod(extract_value_of_opernad(rhs)->get_data())), operand_number));
				break;
			case op_div:
				if (rhs == 0) {
					std::cout << "divided by zero error.";
					exit(EXIT_FAILURE);
				}
				this->stack->push(new Operand(std::to_string(
					std::stod(extract_value_of_opernad(lhs)->get_data())
					/ std::stod(extract_value_of_opernad(rhs)->get_data())), operand_number));
				break;
			case op_pow:
				this->stack->push(new Operand(std::to_string(
					pow(std::stod(extract_value_of_opernad(lhs)->get_data()), std::stod(extract_value_of_opernad(rhs)->get_data()))), operand_number));
				break;

			case op_mod: {
				if (rhs == 0) {
					std::cout << "divided by zero error.";
					exit(EXIT_FAILURE);
				}
				this->stack->push(new Operand(std::to_string(
					(int)std::stod(extract_value_of_opernad(lhs)->get_data())
					% (int)std::stod(extract_value_of_opernad(rhs)->get_data())), operand_number));
				break;
			}

			case op_greater: {
				bool result = std::stod(extract_value_of_opernad(lhs)->get_data()) < std::stod(extract_value_of_opernad(rhs)->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_lesser: {
				bool result = std::stod(extract_value_of_opernad(lhs)->get_data()) > std::stod(extract_value_of_opernad(rhs)->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_eq_lesser: {
				bool result = std::stod(extract_value_of_opernad(lhs)->get_data()) >= std::stod(extract_value_of_opernad(rhs)->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_eq_greater: {
				bool result = std::stod(extract_value_of_opernad(lhs)->get_data()) <= std::stod(extract_value_of_opernad(rhs)->get_data());
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_equal: {
				bool result = extract_value_of_opernad(lhs)->get_data() == extract_value_of_opernad(rhs)->get_data();
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_not_equal: {
				bool result = extract_value_of_opernad(lhs)->get_data() != extract_value_of_opernad(rhs)->get_data();
				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}

			case op_or: {
				bool _lhs = extract_value_of_opernad(lhs)->get_data() == "true";
				bool _rhs = extract_value_of_opernad(rhs)->get_data() == "true";
				bool result = _lhs || _rhs;

				this->stack->push(new Operand(result ? "true" : "false", operand_bool));

				break;
			}

			case op_and: {
				bool _lhs = extract_value_of_opernad(lhs)->get_data() == "true";
				bool _rhs = extract_value_of_opernad(rhs)->get_data() == "true";
				bool result = _lhs && _rhs;

				this->stack->push(new Operand(result ? "true" : "false", operand_bool));
				break;
			}
			}
			delete lhs;
			delete rhs;
			break;
		}

		case op_store_global: {
			Operand* peek = this->stack->peek();
			this->stack->pop();
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);

			if (vm->global_area.find(id) != vm->global_area.end())
				vm->global_area[id] = peek;
			else
				vm->global_area.insert(std::make_pair(id, peek));

			break;
		}

		case op_store_class: {
			Operand* peek = this->stack->peek();
			this->stack->pop();
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);

			if (caller_class->member_variables.find(id) != caller_class->member_variables.end())
				caller_class->member_variables[id] = peek;
			else
				caller_class->member_variables.insert(std::make_pair(id, peek));

			break;
		}

		case op_store_local: {
			Operand* peek = this->stack->peek();
			this->stack->pop();
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);

			if (local_area.find(id) != local_area.end())
				local_area[id] = peek;
			else
				local_area.insert(std::make_pair(id, peek));

			break;
		}

		case op_array_get: {
			Operand* index_peek = this->stack->peek();
			this->stack->pop();
			int index = std::stoi(extract_value_of_opernad(index_peek)->get_data());

			Operand* array_operand = this->stack->peek();
			this->stack->pop();

			Operand* result = extract_value_of_opernad(array_operand)->get_array_data()[index];

			this->stack->push(result);

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
				Operand* peek = this->stack->peek();
				this->stack->pop();

				elements.push_back(peek);
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

		case op_call_global: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);
			CMFunction* code_memory = vm->global_functions[id];

			run_function(vm, nullptr, this, code_memory, parameter_count);

			break;
		}

		case op_call_builtin: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);

			if (id == 0) { // print
				for (int i = 0; i < parameter_count; i++) {
					Operand* _data = this->stack->peek();
					this->stack->pop();

					Operand* data = extract_value_of_opernad(_data);
					operand_type type = data->get_type();

					std::string content = data->get_data();

					if (type == operand_number) {
						printf("%g", std::stod(content));
					}
					else {
						std::cout << content;
					}

					delete _data;
				}
			}
			else if (id == 1) { // window
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
			else if (id == 2) { // load_scene
				for (int i = 0; i < parameter_count; i++) {
					Operand* _target = this->stack->peek();
					this->stack->pop();

					Operand* target = extract_value_of_opernad(_target);

					operand_type type = target->get_type();
					Memory* scene = reinterpret_cast<Memory*>(std::stoull(target->get_data()));

					vm->current_scene_memory = scene;

					delete _target;
				}
			}
			else if (id == 3) { // image
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

				float _x = std::stof(extract_value_of_opernad(position->get_array_data()[0])->get_data()),
					_y = std::stof(extract_value_of_opernad(position->get_array_data()[1])->get_data());

				float f_width = std::stof(extract_value_of_opernad(width)->get_data()), f_height = std::stof(extract_value_of_opernad(height)->get_data());

				Memory* memory = reinterpret_cast<Memory*>(std::stoull(vm->global_area[0]->get_data()));
				CMShader* shader_cm = (CMShader*)memory->get_cm_class();

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

			break;
		}

		case op_load_attr: {
			Operand* target = this->stack->peek();
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

			Operand* target = this->stack->peek();
			this->stack->pop();

			Memory* memory = reinterpret_cast<Memory*>(std::stoull(target->get_data()));
			CMClass* cm = memory->get_cm_class();

			run_function(vm, memory, this, code_memory, parameter_count);

			break;
		}

		}
	}
	delete this;
}