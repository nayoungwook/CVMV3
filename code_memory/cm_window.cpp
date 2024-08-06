#include "cm_window.h"

void register_source_code(CVM* vm, std::string const& loaded_file) {
	std::string name = loaded_file;
	std::wstring wname;
	wname.assign(name.begin(), name.end());

	std::vector<std::string> file = get_file(name);

	std::vector<Token*> parsed_tokens = parse_tokens(file);

	while (!parsed_tokens.empty()) {
		CodeMemory* code_memory = get_code_memory(vm->load_queue, vm->label_id, parsed_tokens);

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

			// find if class already exist.
			vm->global_class.insert(std::make_pair(cm_c->get_id(), cm_c));

			for (int i = 0; i < vm->heap_area.size(); i++) {
				if (vm->heap_area[i]->get_cm_class()->name == cm_c->name) {
					CMFunction* initializer = vm->heap_area[i]->get_cm_class()->initializer;
					run_function(vm, vm->heap_area[i], nullptr, initializer, 0);

					vm->heap_area[i]->set_cm_class(cm_c);

					if (cm_c->get_type() == code_object) {
						unsigned int render_function_id = ((CMObject*)cm_c)->get_render_function_id();
						std::vector<Operator*> temp1;
						std::vector<std::string> temp2;
						CMFunction* render_function = new CMRender(temp1, render_function_id, temp2);
						if (cm_c->member_functions->find(render_function_id) != cm_c->member_functions->end())
							cm_c->member_functions->erase(cm_c->member_functions->find(render_function_id));

						cm_c->member_functions->insert(std::make_pair(render_function_id, render_function));
					}
				}
			}
		}
	}

	CHESTNUT_LOG(L"code refreshed : " + wname, log_level::log_okay);
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
				std::string cir = ".cir";
				std::string cn = ".cn";
				std::string str_path;
				str_path.assign(path.begin(), path.end());

				if (str_path[0] == '#') break;

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
					changed_files.push(std::string(path.begin(), path.end()));
				}

				if (is_cn) {
					CHESTNUT_LOG(L"cn File modified : " + path.substr(0, path.length() - 3), log_level::log_default);

					std::wstring wcommand = get_current_directory() + L"\\"
						+ L"chestnutcompiler -compile " + path.substr(0, path.length() - 3);
					std::string command;
					command.assign(wcommand.begin(), wcommand.end());

					system(command.c_str());
				}

				break;
			}
		}
	);

	while (_running) {
		//	if (current_ticks - backup_ticks != 0)
		//		std::cout << 1000 / (current_ticks - backup_ticks) << std::endl;

		backup_ticks = current_ticks;
		tick_count = SDL_GetTicks();
		current_ticks = tick_count;

		Uint32 start_time = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				_running = false;
				break;
			case SDL_KEYDOWN:
			{
				std::string key = SDL_GetKeyName(event.key.keysym.sym);
				vm->key_data.insert(to_upper_all(key));
				break;
			}
			case SDL_KEYUP: {
				std::string key = SDL_GetKeyName(event.key.keysym.sym);
				vm->key_data.erase(to_upper_all(key));
				break;
			}
			}
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

			CMFunction* render_function =
				current_scene_cm->member_functions->find(render_funciton_id)->second;
			run_function(vm, vm->current_scene_memory, nullptr, render_function, 0);

		}

		while (!changed_files.empty()) {
			CHESTNUT_LOG(L"File refreshed", log_level::log_default);
			register_source_code(vm, changed_files.front());
			changed_files.pop();
		}

		SDL_GL_SwapWindow(window);
		if ((1000 / 60) > (SDL_GetTicks() - start_time))
		{
			SDL_Delay((1000 / 60) - (SDL_GetTicks() - start_time)); //Yay stable framerate!
		}
	}
}

SDL_Window* create_window(std::string const& title, int width, int height) {
	SDL_Window* result = SDL_CreateWindow(title.c_str(), 100, 100, width, height, SDL_WINDOW_OPENGL);

	if (context == nullptr)
		context = SDL_GL_CreateContext(result);

	int init_status = glewInit();
	if (init_status != GLEW_OK) {
		exit(1);
	}

	glGetError();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return result;
}

SDL_Window* CMWindow::get_window() {
	return this->_window;
}

void load_default_shader(CVM* vm) {
	CMShader* cm = new CMShader("fragment.glsl", "transform.glsl");

	cm->register_uniform_data("uWorldTransform");
	cm->register_uniform_data("uViewProj");

	Memory* shader_memory = new Memory((CMClass*)cm);

	vm->global_area.insert(std::make_pair(0, create_address_operand(shader_memory)));
}

CMWindow::CMWindow(CVM* vm, std::string const& title, int width, int height)
	: CMClass(0, 0, -1, -1, -1) {
	this->_window = create_window(title, width, height);

	load_images(vm->load_queue, vm->resources);
	load_default_shader(vm);

	window_loop(vm, this->_window);
}
