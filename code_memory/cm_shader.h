#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../sys_logger.h"
#include "../sys_file.h"
#include "cm_class.h"
#include "../Math.h"

class CMShader : public CodeMemory {
private:
	unsigned int program;
	std::unordered_map<std::string, unsigned int> uniform_data;
public:
	CMShader(std::string const& _frag_path, std::string const& _vert_path);

	std::unordered_map<std::string, unsigned int> get_uniform_data();
	unsigned int get_program() const;
	bool compile_succeeded(unsigned int shader);
	bool compile_shader(unsigned int& _shader, const std::string& path, GLenum type);
	void register_uniform_data(const char* name);
	void set_matrix_uniform(const char* name, Matrix4 matrix);
};

unsigned int create_program(unsigned int frag, unsigned int vert);