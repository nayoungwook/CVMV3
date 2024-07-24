#include "cm_window.h"

void window_loop(CVM* vm, SDL_Window* window) {
	SDL_Event event;

	bool _running = true;
	int tick_count = 0;
	int backup_ticks = 0, current_ticks = 0;

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
				/*
							case SDL_KEYDOWN:
							{
								std::string key = SDL_GetKeyName(event.key.keysym.sym);
								machine->key_data.insert(to_upper_all(key));
								break;
							}

							case SDL_KEYUP: {
								std::string key = SDL_GetKeyName(event.key.keysym.sym);
								machine->key_data.erase(to_upper_all(key));
								break;
							}
				*/
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
