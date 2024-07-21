#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <iostream>
#include "cm_function.h"
#include "cm_class.h"
#include "cm_scene.h"
#include "memory.h"
#include "cvm.h"
#include "function_frame.h"
#include "cm_image.h"

class CMWindow : public CMClass {
private:
	SDL_Window* _window;
public:
	SDL_Window* get_window();
	CMWindow(CVM* vm, std::string const& title, int width, int height);
};

static SDL_GLContext context;

SDL_Window* create_window(std::string const& title, int width, int height);
void window_loop(CVM* vm, SDL_Window* window);
void load_default_shader(CVM* vm);
