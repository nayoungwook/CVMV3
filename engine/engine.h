#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include "../Math.h"
#include "../code_memory/cm_shader.h"
#include <SDL_ttf.h>
#include <string>
#include "../memory.h"
#include "../code_memory/cm_image.h"

void initialize_engine();
void render_image(CMShader* shader, unsigned int texture_id, unsigned int vao, float x, float y, float width, float height, float rotation, int proj_width, int proj_height);
void render_text(TTF_Font* font, CMShader* shader, std::wstring const& str, float x, float y, int _r, int _g, int _b, int _a, float rotation, int proj_width, int proj_height, int size);