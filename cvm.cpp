#include "cvm.h"
#include "function_frame.h"

#undef main

Memory* CVM::current_scene_memory = nullptr;

void register_parsed_file(std::vector<Token*>& parsed_tokens, CVM* vm) {

	while (!parsed_tokens.empty()) {
		CodeMemory* code_memory = get_code_memory(vm, parsed_tokens);

		if (code_memory == nullptr) continue;

		if (code_memory->get_type() == code_function) {
			CMFunction* cm_f = ((CMFunction*)code_memory);

			vm->global_functions.insert(std::make_pair(cm_f->get_id(), cm_f));
		}
		else if (
			code_memory->get_type() == code_class ||
			code_memory->get_type() == code_scene || code_memory->get_type() == code_object) {
			CMClass* cm_c = ((CMClass*)code_memory);

			vm->global_class.insert(std::make_pair(cm_c->get_id(), cm_c));
		}
	}
}

int main() {

	initialize_engine();

	CVM* vm = new CVM();
	CMFunction* main_function = nullptr;

	vm->label_id = new std::unordered_map<std::string, unsigned int>;

	std::string path = "main.cir";
	std::vector<std::string> file = get_file(path);
	std::vector<Token*> parsed_tokens = parse_tokens(file);

	vm->imported_files.insert("main");
	register_parsed_file(parsed_tokens, vm);

	std::unordered_map<unsigned int, CMFunction*>::iterator global_function_iterator = vm->global_functions.begin();

	for (; global_function_iterator != vm->global_functions.end(); global_function_iterator++) {
		if (global_function_iterator->second->name == "main") {
			main_function = global_function_iterator->second;
		}
	}

	if (main_function == nullptr) {
		CHESTNUT_LOG(L"Main Function not found. Please deaclare it.", log_level::log_error);
		exit(EXIT_FAILURE);
	}

	FunctionFrame* main_frame = new FunctionFrame(main_function);
	main_frame->run(vm, nullptr, nullptr);

	return 0;
}