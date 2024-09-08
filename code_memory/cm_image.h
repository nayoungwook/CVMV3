#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <queue>
#include <iostream>
#include "../error/sys_error.h"
#include "cm_class.h"
#include "../engine/stb_image.h"

class CMImage : public CodeMemory {
private:
	unsigned int texture, vao;
public:
	unsigned int get_texture() const;
	unsigned int get_vao() const;
	CMImage(unsigned int texture, unsigned int vao);
};

void load_images(std::queue<std::pair<std::string, std::string>>& load_queue, std::unordered_map<std::string, CMImage*>& resources);
std::pair<unsigned int, unsigned int> create_texture_id(std::string const& path, int width, int height, int channel);