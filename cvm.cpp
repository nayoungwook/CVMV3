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

void load_builtin_array(CVM* vm) {
	unsigned int builtin_id = vm->builtin_class.size();

	CMArray* cm = new CMArray(builtin_id, 0);

	vm->array_code_memory_id = builtin_id;

	vm->builtin_class.insert(std::make_pair(builtin_id, cm));
}

CVM::CVM() {
	this->gc = new CGC(this->stack_area, this->heap_area, this->global_area);
	load_builtin_array(this);
}


int main(int argc, char* args[]) {

	initialize_engine();

	CVM* vm = new CVM();

	CMFunction* main_function = nullptr;

	vm->label_id = new std::unordered_map<std::wstring, unsigned int>;

	if (argc == 1) {
		CHESTNUT_LOG(L"With no arguments, the process is on the \'debug mode\'", log_level::log_warn);
		CHESTNUT_LOG(L"If it was not your intention, please type \'chestnut -help.\' to get help.", log_level::log_warn);

		std::wstring path = L"main.cir";
		std::vector<std::wstring> file = get_file(path);
		std::vector<Token*> parsed_tokens = parse_tokens(file);

		vm->imported_files.insert(L"main");
		register_parsed_file(parsed_tokens, vm);

	}
	else if (argc == 2) {
		std::string file_name = args[1];

		std::wstring path = std::wstring(file_name.begin(), file_name.end()) + L".cir";
		std::vector<std::wstring> file = get_file(path);
		std::vector<Token*> parsed_tokens = parse_tokens(file);

		vm->imported_files.insert(L"main");
		register_parsed_file(parsed_tokens, vm);

	}

	std::unordered_map<unsigned int, CMFunction*>::iterator global_function_iterator = vm->global_functions.begin();

	for (; global_function_iterator != vm->global_functions.end(); global_function_iterator++) {
		if (global_function_iterator->second->name == L"main") {
			main_function = global_function_iterator->second;
		}
	}

	if (main_function == nullptr) {
		CHESTNUT_LOG(L"Main Function not found. Please deaclare it.", log_level::log_error);
		exit(EXIT_FAILURE);
	}

	std::time_t start = time(NULL);
	std::time_t fin = time(NULL);

	time(&start);

	FunctionFrame* main_frame = new FunctionFrame(main_function);
	main_frame->run(vm, nullptr, nullptr);

	time(&fin);

	std::wcout << std::endl;
	std::wstring diff = std::to_wstring((int)difftime(fin, start));
	CHESTNUT_LOG(L"Process end with : " + std::wstring(diff.begin(), diff.end()) + L"sec.", log_level::log_default);

	return 0;
}