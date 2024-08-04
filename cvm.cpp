#include "cvm.h"
#include "function_frame.h"

#undef main

Memory* CVM::current_scene_memory = nullptr;

int main() {

	std::string path = "main.cir";
	std::vector<std::string> file = get_file(path);
	std::vector<Token*> parsed_tokens;

	for (std::string _f : file) {
		std::vector<Token*> tkns = extract_tokens(_f);

		for (Token* tkn : tkns) {
			parsed_tokens.push_back(tkn);
		}
	}

	initialize_engine();

	CVM* vm = new CVM();
	CMFunction* main_function = nullptr;

	vm->label_id = new std::unordered_map<std::string, unsigned int>;

	while (!parsed_tokens.empty()) {
		CodeMemory* code_memory = get_code_memory(vm->load_queue, vm->label_id, parsed_tokens);

		if (code_memory == nullptr) continue;

		if (code_memory->get_type() == code_function) {
			CMFunction* cm_f = ((CMFunction*)code_memory);

			if (cm_f->name == "main") {
				main_function = cm_f;
			}

			vm->global_functions.insert(std::make_pair(cm_f->get_id(), cm_f));
		}
		else if (code_memory->get_type() == code_class || code_memory->get_type() == code_scene || code_memory->get_type() == code_object) {
			CMClass* cm_f = ((CMClass*)code_memory);

			vm->global_class.insert(std::make_pair(cm_f->get_id(), cm_f));
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