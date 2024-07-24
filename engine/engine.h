#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include "../Math.h"
#include "../code_memory/cm_shader.h"

void initialize_engine();
void render_image(CMShader* shader, unsigned int texture_id, unsigned int vao, float x, float y, float width, float height, float rotation);
