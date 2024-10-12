#include "cm_window.h"

void register_render_function_code(CMClass* cm_c) {
	unsigned int render_function_id = ((CMObject*)cm_c)->get_render_function_id();
	std::vector<Operator*> temp1;
	std::vector<std::wstring> temp2;
	CMFunction* render_function = new CMRender(temp1, render_function_id, temp2);
	if (cm_c->member_functions->find(render_function_id) != cm_c->member_functions->end())
		cm_c->member_functions->erase(cm_c->member_functions->find(render_function_id));

	cm_c->member_functions->insert(std::make_pair(render_function_id, render_function));
}

void register_source_code(CVM* vm, std::wstring const& loaded_file) {
	std::wstring name = loaded_file;
	std::wstring wname;
	wname.assign(name.begin(), name.end());

	std::vector<std::wstring> file = get_file(name);

	std::vector<Token*> parsed_tokens = parse_tokens(file);

	while (!parsed_tokens.empty()) {
		CodeMemory* code_memory = get_code_memory(vm, parsed_tokens);

		if (code_memory == nullptr) continue;

		if (code_memory->get_type() == code_function) {
			CMFunction* cm_f = ((CMFunction*)code_memory);
			std::unordered_map<unsigned int, CMFunction*>::iterator global_function_iterator;

			bool function_exist = false;

			for (global_function_iterator = vm->global_functions.begin();
				global_function_iterator != vm->global_functions.end(); global_function_iterator++) {
				function_exist = true;
			}

			if (function_exist) {
				vm->global_functions[cm_f->get_id()] = cm_f;
			}
			else {
				vm->global_functions.insert(std::make_pair(cm_f->get_id(), cm_f));
			}
		}
		else if (
			code_memory->get_type() == code_class ||
			code_memory->get_type() == code_scene || code_memory->get_type() == code_object) {

			CMClass* cm_c = ((CMClass*)code_memory);

			std::unordered_map<unsigned int, CMClass*>::iterator global_class_iterator;

			bool class_already_exist = false;

			for (global_class_iterator = vm->global_class.begin(); global_class_iterator != vm->global_class.end(); global_class_iterator++) {
				if (global_class_iterator->second->name == cm_c->name) {
					global_class_iterator->second = cm_c;
					class_already_exist = true;
					break;
				}
			}

			if (!class_already_exist) {
				vm->global_class.insert(std::make_pair(cm_c->get_id(), cm_c));
			}

			std::unordered_set<CMClass*> old_cm;

			for (int i = 0; i < vm->heap_area.size(); i++) {
				bool cm_changed = vm->heap_area[i]->get_backup_cm_class() != vm->heap_area[i]->get_cm_class();
				Memory* target_memory = vm->heap_area[i];

				if (cm_changed) {
					CMClass* changed_cm = vm->heap_area[i]->get_cm_class();
					std::unordered_map<unsigned int, std::wstring> backup_member_variable_names = vm->heap_area[i]->member_variable_names;

					// backing up memories for check already declared memory ( with name. )
					std::unordered_map<std::wstring, Operand*> backup_members;
					std::unordered_map<unsigned int, Operand*>::iterator member_variable_iterator;

					for (member_variable_iterator = target_memory->member_variables.begin();
						member_variable_iterator != target_memory->member_variables.end(); member_variable_iterator++) {
						//						Operand* backup_op = copy_operand(member_variable_iterator->second);
						backup_members.insert(std::make_pair(member_variable_iterator->second->variable_name, member_variable_iterator->second));
					}

					run_function(vm, vm->heap_area[i], nullptr, changed_cm->initializer, 0);

					// re-register already declared memories
					std::unordered_map<unsigned int, Operand*>::iterator member_variable_name_iterator;
					for (member_variable_name_iterator = target_memory->member_variables.begin();
						member_variable_name_iterator != target_memory->member_variables.end();
						member_variable_name_iterator++) {
						if (backup_members.find(member_variable_name_iterator->second->variable_name) != backup_members.end()) {
							Operand* changed_memory = member_variable_name_iterator->second;
							member_variable_name_iterator->second = backup_members[member_variable_name_iterator->second->variable_name];
							//backup_members[member_variable_name_iterator->second->variable_name] = nullptr; // mark used member variable
						}
					}

					// remove unused backup members
					/*
					std::unordered_map<std::wstring, Operand*>::iterator backup_member_remover;
					for (backup_member_remover = backup_members.begin();
						backup_member_remover != backup_members.end(); backup_member_remover++) {
						if (backup_member_remover->second != nullptr)
							delete backup_member_remover->second;
					}
					*/
				}
				old_cm.insert(vm->heap_area[i]->get_backup_cm_class());
				vm->heap_area[i]->update_backup_cm_class();
			}


			if (cm_c->get_type() == code_object)
				register_render_function_code(cm_c);

			std::wstring w_object_name;
			w_object_name.assign(cm_c->name.begin(), cm_c->name.end());

			if (cm_c->get_type() == code_object)
				CHESTNUT_LOG(L"object reloaded : " + w_object_name, log_level::log_okay);
			else if (cm_c->get_type() == code_scene)
				CHESTNUT_LOG(L"scene reloaded : " + w_object_name, log_level::log_okay);
			else if (cm_c->get_type() == code_class)
				CHESTNUT_LOG(L"class reloaded : " + w_object_name, log_level::log_okay);
		}
	}

	CHESTNUT_LOG(L"code refreshed : " + wname, log_level::log_okay);
}

void log_data(CVM* vm) {
	// check heap usage
	//CHESTNUT_LOG(L"Current Heap Usage : " + std::to_wstring(vm->heap_area.size()), log_level::log_default);

	//		if (current_ticks - backup_ticks != 0)
	//			std::cout << 1000 / (current_ticks - backup_ticks) << std::endl;
}

void window_loop(CVM* vm, SDL_Window* window) {
	SDL_Event event;

	bool _running = true;
	int tick_count = 0;
	int backup_ticks = 0, current_ticks = 0;

	filewatch::FileWatch<std::wstring> watch(
		get_current_directory(),
		[](const std::wstring& path, const filewatch::Event change_type) {
			switch (change_type)
			{
			case filewatch::Event::modified:
				bool is_cir = true;
				bool is_cn = true;
				std::wstring cir = L".cir";
				std::wstring cn = L".cn";
				std::wstring str_path;
				str_path.assign(path.begin(), path.end());

				if (!(std::isalpha(str_path[0]) || str_path[0] == '_')) break;

				if (str_path.length() >= 3) {
					for (int i = 3; i >= 0; i--) {
						if (str_path[str_path.length() - 1 - i] != cir[3 - i]) {
							is_cir = false;
						}

						if (i != 3) {
							if (str_path[str_path.length() - 1 - i] != cn[2 - i]) {
								is_cn = false;
							}
						}
					}
				}
				else {
					is_cir = false;
				}

				if (is_cir) {
					CHESTNUT_LOG(L"cir File modified : " + path, log_level::log_default);

					changed_files.push_back(std::wstring(path.begin(), path.end()));
				}

				if (is_cn) {
					CHESTNUT_LOG(L"cn File modified : " + path.substr(0, path.length() - 3), log_level::log_default);

					std::wstring wcommand = L"chestnutcompiler -compile " + path.substr(0, path.length() - 3);
					std::wstring command;
					command.assign(wcommand.begin(), wcommand.end());

					system(std::string(command.begin(), command.end()).c_str());
				}

				break;
			}
		}
	);

	float log_timer = 0;

	while (_running) {
		backup_ticks = current_ticks;
		tick_count = SDL_GetTicks();
		current_ticks = tick_count;

		Uint32 start_time = SDL_GetTicks();

		log_timer += 0.025f;

		if (log_timer >= 1) {
			log_timer = 0;
			log_data(vm);
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				_running = false;
				break;
			case SDL_KEYDOWN:
			{
				std::string key = SDL_GetKeyName(event.key.keysym.sym);
				vm->key_data.insert(to_upper_all(std::wstring(key.begin(), key.end())));
				break;
			}
			case SDL_KEYUP: {
				std::string key = SDL_GetKeyName(event.key.keysym.sym);
				vm->key_data.erase(to_upper_all(std::wstring(key.begin(), key.end())));
				break;
			}
			case SDL_MOUSEMOTION: {
				int win_w, win_h;
				SDL_GetWindowSize(window, &win_w, &win_h);
				int mouse_x = event.motion.x - win_w / 2, mouse_y = event.motion.y - win_h / 2;
				vm->global_area[1]->get_array_data()->at(0)->set_data(std::to_wstring(mouse_x));
				vm->global_area[1]->get_array_data()->at(1)->set_data(std::to_wstring(mouse_y));
			}
			}
		}

		while (!changed_files.empty()) {
			CHESTNUT_LOG(L"File refreshed", log_level::log_default);
			register_source_code(vm, changed_files.front());
			changed_files.erase(changed_files.begin());
		}

		glClearColor(0.05f, 0.05f, 0.06f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (vm->current_scene_memory != nullptr) {
			CMScene* current_scene_cm = (CMScene*)vm->current_scene_memory->get_cm_class();

			unsigned int init_funciton_id = current_scene_cm->get_init_function_id();
			unsigned int tick_funciton_id = current_scene_cm->get_tick_function_id();
			unsigned int render_funciton_id = current_scene_cm->get_render_function_id();

			CMScene* scene = current_scene_cm;

			CMFunction* tick_function =
				current_scene_cm->member_functions->find(tick_funciton_id)->second;
			run_function(vm, vm->current_scene_memory, nullptr, tick_function, 0);
		}

		if (vm->current_scene_memory != nullptr) {
			CMScene* current_scene_cm = (CMScene*)vm->current_scene_memory->get_cm_class();

			unsigned int init_funciton_id = current_scene_cm->get_init_function_id();
			unsigned int tick_funciton_id = current_scene_cm->get_tick_function_id();
			unsigned int render_funciton_id = current_scene_cm->get_render_function_id();

			CMScene* scene = current_scene_cm;

			CMFunction* render_function =
				current_scene_cm->member_functions->find(render_funciton_id)->second;
			run_function(vm, vm->current_scene_memory, nullptr, render_function, 0);
		}

		SDL_GL_SwapWindow(window);
		if ((1000 / 60) > (SDL_GetTicks() - start_time))
		{
			SDL_Delay((1000 / 60) - (SDL_GetTicks() - start_time)); //Yay stable framerate!
		}
	}
}

SDL_Window* create_window(std::wstring const& title, int width, int height) {
	SDL_Window* result = SDL_CreateWindow(std::string(title.begin(), title.end()).c_str(), 100, 100, width, height, SDL_WINDOW_OPENGL);

	if (context == nullptr)
		context = SDL_GL_CreateContext(result);

	int init_status = glewInit();
	if (init_status != GLEW_OK) {
		CHESTNUT_THROW_ERROR(L"Failed to initialize glew! please check your opengl libraries.", "FAILED_TO_INIT_GLEW", "0x10", -1);
	}

	glGetError();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize Font
	if (TTF_Init() < 0) {
		CHESTNUT_THROW_ERROR(L"Failed to initialize SDL TTF! please check your SDL libraries.", "FAILED_TO_INIT_TTF", "0x11", -1);
	}

	return result;
}

SDL_Window* CMWindow::get_window() {
	return this->_window;
}

void load_default_shader(CVM* vm) {
	unsigned int builtin_id = vm->builtin_class.size();

	CMShader* cm = new CMShader(builtin_id, L"fragment.glsl", L"transform.glsl");

	cm->register_uniform_data("uWorldTransform");
	cm->register_uniform_data("uViewProj");

	vm->builtin_class.insert(std::make_pair(builtin_id, cm));

	Memory* shader_memory = new Memory(vm->builtin_class.find(builtin_id));

	vm->global_area.insert(std::make_pair(0, create_address_operand(shader_memory)));
}

void load_builtin_variables(CVM* vm) {
	// for mouse
	std::vector<Operand*>* mouse_elements = new std::vector<Operand*>;
	for (int i = 0; i < 2; i++) {
		mouse_elements->push_back(new Operand(L"0", operand_number));
	}
	Operand* mouse_memory = new Operand(mouse_elements, operand_vector);
	vm->global_area.insert(std::make_pair(1, mouse_memory));
}

CMWindow::CMWindow(unsigned int id, CVM* vm, std::wstring const& title, int width, int height)
	: CMClass(id, 0, -1, -1, -1) {
	this->_window = create_window(title, width, height);
	this->name = L"window";

	vm->renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if (vm->renderer == NULL) {
		CHESTNUT_THROW_ERROR(L"Failed to create renderer!", "FAILED_TO_CREATE_RENDERER", "0x12", -1);
	}

	load_images(vm->load_queue, vm->resources);
	load_fonts(vm->font_queue, vm->font_resources);
	load_default_shader(vm);

	load_builtin_variables(vm);

	window_loop(vm, this->_window);

}