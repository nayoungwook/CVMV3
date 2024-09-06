#include "cm_shader.h"

CMShader::CMShader(unsigned int id, std::string const& _frag_path, std::string const& _vert_path) : CMClass(id, 0, -1, -1, -1) {
	unsigned int frag = 0, vert = 0;

	std::string frag_path = get_absolute_path(_frag_path);
	std::string vert_path = get_absolute_path(_vert_path);

	if (!compile_shader(frag, frag_path, GL_FRAGMENT_SHADER)) {
		std::wstring msg = L"Compilation error in shader : ";
		msg.assign(frag_path.begin(), frag_path.end());

		CHESTNUT_LOG(msg, log_level::log_error);
	}
	else {
		std::wstring msg = L"Compilation succeed in shader : ";
		msg.assign(frag_path.begin(), frag_path.end());
		CHESTNUT_LOG(msg, log_level::log_success);
	}

	if (!compile_shader(vert, vert_path, GL_VERTEX_SHADER)) {
		std::wstring msg = L"Compilation error in shader : ";
		msg.assign(vert_path.begin(), vert_path.end());

		CHESTNUT_LOG(msg, log_level::log_error);
	}
	else {
		std::wstring msg = L"Compilation succeed in shader : ";
		msg.assign(vert_path.begin(), vert_path.end());
		CHESTNUT_LOG(msg, log_level::log_success);
	}

	this->program = create_program(frag, vert);
}

void CMShader::register_uniform_data(const char* name) {
	unsigned int loc = glGetUniformLocation(this->program, name);
	this->uniform_data.insert(std::make_pair(name, loc));
}

void CMShader::set_matrix_uniform(const char* name, Matrix4 matrix) {
	if (this->uniform_data.find(name) == this->uniform_data.end()) {
		std::cout << "Error! uniform " << name << " not found." << std::endl;
		exit(1);
	}

	glUniformMatrix4fv(
		this->uniform_data[name],
		1,
		GL_TRUE,
		matrix.GetAsFloatPtr()
	);
}

std::unordered_map<std::string, unsigned int> CMShader::get_uniform_data() {
	return this->uniform_data;
}

bool CMShader::compile_succeeded(unsigned int shader) {
	int status;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		fprintf(stderr, "Compilation error in shader : %s\n", strInfoLog);
		delete[] strInfoLog;

		return false;
	}

	return true;
}

bool CMShader::compile_shader(unsigned int& _shader, const std::string& path, GLenum type) {
	std::ifstream shader_file(path);

	if (shader_file.is_open()) {
		std::stringstream str_stream;

		str_stream << shader_file.rdbuf();

		std::string content = str_stream.str();
		const char* content_char = content.c_str();

		unsigned int result = 0;
		result = glCreateShader(type);

		glShaderSource(result, 1, &(content_char), nullptr);
		glCompileShader(result);

		if (!compile_succeeded(result)) {
			SDL_Log("Failed to compile shader : %s", path.c_str());
			return false;
		}

		_shader = result;
	}
	else {
		SDL_Log("Failed to find shader : %s", path.c_str());
		return false;
	}

	return true;
}

unsigned int CMShader::get_program() const {
	return program;
}

unsigned int create_program(unsigned int frag, unsigned int vert) {
	unsigned int result = glCreateProgram();

	glAttachShader(result, frag);
	glAttachShader(result, vert);

	glLinkProgram(result);
	glUseProgram(result);

	return result;
}
