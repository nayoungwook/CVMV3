#include "cm_image.h"

CMImage::CMImage(unsigned int texture, unsigned int vao) : texture(texture), vao(vao) {

}

unsigned int CMImage::get_texture() const {
	return this->texture;
}

unsigned int CMImage::get_vao() const {
	return this->vao;
}

void load_images(std::queue<std::pair<std::string, std::string>>& load_queue, std::unordered_map<std::string, CMImage*>& resources) {
	while (!load_queue.empty()) {
		std::pair<std::string, std::string> q = load_queue.front();
		load_queue.pop();

		std::pair<unsigned int, unsigned int> result = create_texture_id(q.second, 128, 128, 0);

		if (resources.find(q.first) == resources.end()) {
			resources.insert(std::make_pair(q.first, new CMImage(result.first, result.second)));
		}
		else {
			resources[q.first] = new CMImage(result.first, result.second);
		}
	}
}

unsigned int create_vao() {
	unsigned char* bytes;
	unsigned int vertex_count = 0;
	unsigned int indices_count;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	try {
		
		float vertices[] = {
			-0.5f, 0.5f, 0.0f,   0.f, 0.f,
			0.5f, 0.5f, 0.0f,    1.f, 0.f,
			0.5f, -0.5f, 0.0f,   1.f, 1.f,
			-0.5f, -0.5f, 0.0f,  0.f, 1.f,
		};

		int position_vertex_count = 3, tex_coord_count = 2;
		vertex_count = position_vertex_count + tex_coord_count;

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
		};

		indices_count = sizeof(indices) / sizeof(unsigned int);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glBufferData(GL_ARRAY_BUFFER, vertex_count * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, position_vertex_count, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_count, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, tex_coord_count, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_count, (void*)(sizeof(float) * position_vertex_count));
	}
	catch (int e) {
		std::cout << "Error at creating vao... " << std::endl;
		exit(e);
	}

	return vao;
}

std::pair<unsigned int, unsigned int> create_texture_id(std::string const& path, int width, int height, int channel) {

	unsigned char* bytes;
	unsigned int vertex_count = 0;
	unsigned int indices_count;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;
	unsigned int texture;

	bytes = stbi_load(path.c_str(), &width, &height, &channel, 4);

	if (!bytes) {
		std::string file_name = path;
		CHESTNUT_THROW_ERROR(L"Failed to load image " + std::wstring(file_name.begin(), file_name.end()) + L" Please check file name again.",
			"RUNTIME_FAILED_TO_LOAD_IMAGE", "0x08", -1);
	}

	try {
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		float vertices[] = {
			-0.5f, 0.5f, 0.0f,   0.f, 0.f,
			0.5f, 0.5f, 0.0f,    1.f, 0.f,
			0.5f, -0.5f, 0.0f,   1.f, 1.f,
			-0.5f, -0.5f, 0.0f,  0.f, 1.f,
		};

		int position_vertex_count = 3, tex_coord_count = 2;
		vertex_count = position_vertex_count + tex_coord_count;

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,
		};

		indices_count = sizeof(indices) / sizeof(unsigned int);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glBufferData(GL_ARRAY_BUFFER, vertex_count * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, position_vertex_count, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_count, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, tex_coord_count, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_count, (void*)(sizeof(float) * position_vertex_count));
	}
	catch (int e) {
		std::cout << "Error at image loading... " << std::endl;
		exit(e);
	}

	return std::make_pair(texture, vao);
}