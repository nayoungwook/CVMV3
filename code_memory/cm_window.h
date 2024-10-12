#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <iostream>
#include <unordered_set>
#include "cm_function.h"
#include "cm_class.h"
#include "cm_scene.h"
#include "memory.h"
#include "../cvm.h"
#include "../function_frame.h"
#include "../sys_util.h"
#include "../sys_file.h"
#include "../operand.h"
#include "cm_image.h"

#include "../FileWatcher.hpp"

class CMWindow : public CMClass {
private:
	SDL_Window* _window;
public:
	SDL_Window* get_window();
	CMWindow(unsigned int id, CVM* vm, std::wstring const& title, int width, int height);
};

static SDL_GLContext context;

SDL_Window* create_window(std::wstring const& title, int width, int height);
void window_loop(CVM* vm, SDL_Window* window);
void load_default_shader(CVM* vm);
void load_builtin_variables(CVM* vm);

static std::vector<std::wstring> changed_files;
void register_source_code(CVM* machine, std::wstring const& loaded_file);
void register_render_function_code(CMClass* cm_c);

void log_data(CVM* vm);