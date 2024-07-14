#include "function_frame.h"

CMFunction* FunctionFrame::get_code_memory() const {
	return code_memory;
}

FunctionFrame::FunctionFrame(CMFunction* code_memory) : code_memory(code_memory) {
	this->stack = new Stack();
}

void run_function(CVM* vm, FunctionFrame* caller_frame, CMFunction* code_memory, int parameter_count) {
	FunctionFrame* frame = new FunctionFrame(code_memory);

	for (int i = 0; i < parameter_count; i++) {
		Operand* op = caller_frame->stack->peek();
		caller_frame->stack->pop();
		frame->local_area.insert(std::make_pair(i, op));
	}

	frame->run(vm, caller_frame, nullptr);
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

			if (condition->get_data() == "true") {
				std::string id = op->get_operands()[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			break;
		}

		case op_new: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);

			CMClass* code_memory = vm->global_class[id];

			Memory* memory = new Memory(code_memory);

			// run constructor
			run_function(vm, this, code_memory->constructor, parameter_count);

			// run initializer
			run_function(vm, this, code_memory->initializer, 0);

			// store in heap
			vm->heap_area.push_back(memory);

			this->stack->push(new Operand(std::to_string((unsigned long long)(void**)memory), operand_address));

			break;
		}

		case op_if: {
			Operand* condition = this->stack->peek();
			this->stack->pop();

			if (condition->get_data() == "false") {
				std::string id = op->get_operands()[0]->identifier;
				line = vm->label_id->find(id)->second;
			}

			break;
		}

		case op_inc: {
			Operand* target = this->stack->peek();
			this->stack->pop();

			double v = std::stod(target->get_data()) + 1;
			target->set_data(std::to_string(v));

			break;
		}

		case op_dec: {
			Operand* target = this->stack->peek();
			this->stack->pop();

			double v = std::stod(target->get_data()) - 1;
			target->set_data(std::to_string(v));

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
					std::stod(lhs->get_data()) + std::stod(rhs->get_data())), operand_number));
				break;
			case op_sub:
				this->stack->push(new Operand(std::to_string(
					std::stod(lhs->get_data()) - std::stod(rhs->get_data())), operand_number));
				break;
			case op_mul:
				this->stack->push(new Operand(std::to_string(
					std::stod(lhs->get_data()) * std::stod(rhs->get_data())), operand_number));
				break;
			case op_div:
				if (rhs == 0) {
					std::cout << "divided by zero error.";
					exit(EXIT_FAILURE);
				}
				this->stack->push(new Operand(std::to_string(
					std::stod(lhs->get_data()) / std::stod(rhs->get_data())), operand_number));
				break;
			case op_pow:
				this->stack->push(new Operand(std::to_string(
					pow(std::stod(lhs->get_data()), std::stod(rhs->get_data()))), operand_number));
				break;

			case op_mod: {
				if (rhs == 0) {
					std::cout << "divided by zero error.";
					exit(EXIT_FAILURE);
				}
				this->stack->push(new Operand(std::to_string(
					(int)std::stod(lhs->get_data()) % (int)std::stod(rhs->get_data())), operand_number));
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
			Operand* peek = this->stack->peek();
			this->stack->pop();
			int index = std::stoi(peek->get_data());

			Operand* array_operand = this->stack->peek();
			this->stack->pop();

			Operand* result = array_operand->get_array_data()[index];

			this->stack->push(result);

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

			result = new Operand(elements);

			this->stack->push(result);

			break;
		}

		case op_load_global: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			Operand* found_op = vm->global_area[id];

			this->stack->push(new Operand(found_op->get_data(), found_op->get_type()));

			break;
		}
		case op_load_local: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			Operand* found_op = local_area[id];

			this->stack->push(found_op);
			break;
		}

		case op_call_global: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);
			CMFunction* code_memory = vm->global_functions[id];

			run_function(vm, this, code_memory, parameter_count);

			break;
		}

		case op_call_builtin: {
			unsigned int id = std::stoi(op->get_operands()[0]->identifier);
			int parameter_count = std::stoi(op->get_operands()[1]->identifier);

			if (id == 0) { // print
				for (int i = 0; i < parameter_count; i++) {
					Operand* data = this->stack->peek();
					operand_type type = data->get_type();
					this->stack->pop();

					std::string content = data->get_data();

					if (type == operand_number) {
						printf("%g", std::stod(content));
					}
					else {
						std::cout << content;
					}
				}
			}
			else if (id == 1) { // window
				std::string title;
				int width, height;

				for (int i = 0; i < parameter_count; i++) {
					Operand* data = this->stack->peek();
					operand_type type = data->get_type();
					this->stack->pop();

					if (i == 0) title = data->get_data();
					else if (i == 1) width = (int)std::stod(data->get_data());
					else if (i == 2) height = (int)std::stod(data->get_data());
				}

				CMWindow* cm_window = new CMWindow(vm, title, width, height);
				Memory* win_memory = new Memory(cm_window);

				this->stack->push(new Operand(std::to_string((unsigned long long)(void**)win_memory), operand_address));
			}
			else if (id == 2) { // load_scene
				for (int i = 0; i < parameter_count; i++) {
					Operand* target = this->stack->peek();
					operand_type type = target->get_type();
					this->stack->pop();
					Memory* scene = reinterpret_cast<Memory*>(std::stoull(target->get_data()));

					vm->current_scene_memory = scene;
				}
			}

			break;
		}

		case op_load_attr: {
			Operand* target = this->stack->peek();
			this->stack->pop();

			Memory* memory = reinterpret_cast<Memory*>(std::stoull(target->get_data()));
			this->stack->push(memory->member_variables[std::stoi(op->get_operands()[0]->identifier)]);
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

			run_function(vm, this, code_memory, parameter_count);

			break;
		}

		}
	}
	delete this;
}