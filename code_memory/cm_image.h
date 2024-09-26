#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <queue>
#include <iostream>
#include "../error/sys_error.h"
#include "cm_class.h"
#include "../engine/stb_image.h"
#include <SDL_ttf.h>

class CMImage : public CodeMemory {
private:
	unsigned int texture, vao;
public:
	unsigned int get_texture() const;
	unsigned int get_vao() const;
	CMImage(unsigned int texture, unsigned int vao);
};

void load_images(std::queue<std::pair<std::wstring, std::wstring>>& load_queue, std::unordered_map<std::wstring, CMImage*>& resources);
void load_fonts(std::queue<std::pair<std::wstring, std::wstring>>& font_queue, std::unordered_map<std::wstring, TTF_Font*>& font_resources);
std::pair<unsigned int, unsigned int> create_texture_id(std::wstring const& path, int width, int height, int channel);
unsigned int create_vao();