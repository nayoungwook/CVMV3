#include "engine.h"

void initialize_engine() {
	glewExperimental = GL_TRUE;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

}

void render_image(CMShader* shader, unsigned int texture_id, unsigned int vao, float x, float y, float width, float height, float rotation, int proj_width, int proj_height) {

	SDL_Point rotation_center = {
		(int)width / 2, (int)height / 2
	};

	Matrix4 world_transform = Matrix4::CreateScale(width, height, 1.0f) * Matrix4::CreateRotationZ(-rotation / 180 * Math::Pi) * Matrix4::CreateTranslation(Vector3(x, y, 0));

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_id);

	shader->set_matrix_uniform("uWorldTransform", world_transform);
	shader->set_matrix_uniform("uViewProj", Matrix4::CreateSimpleViewProj(proj_width, proj_height));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glDisable(GL_TEXTURE_2D);
}

void render_text(TTF_Font* font, CMShader* shader, std::string const& str, float x, float y, float _r, float _g, float _b, float _a, float rotation, int proj_width, int proj_height, int size) {

	SDL_Color color = { 255, 150, 100 };
	SDL_Surface* _temp_surface = TTF_RenderUTF8_Solid(font, str.c_str(), color);
	SDL_Surface* surface = SDL_ConvertSurfaceFormat(_temp_surface, SDL_PIXELFORMAT_RGBA8888, 0);
	unsigned int vao, vbo, ebo;

	float vertices[] = {
		-0.5f, 0.5f, 0.0f,   0.f, 0.f,
		0.5f, 0.5f, 0.0f,    1.f, 0.f,
		0.5f, -0.5f, 0.0f,   1.f, 1.f,
		-0.5f, -0.5f, 0.0f,  0.f, 1.f,
	};

	int position_vertex_count = 3, tex_coord_count = 2;
	int vertex_count = position_vertex_count + tex_coord_count;

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};
	
	int indices_count = sizeof(indices) / sizeof(unsigned int);

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

	unsigned int tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Matrix4 world_transform = Matrix4::CreateScale(surface->w / 64 * size, surface->h / 64 * size, 1.0f) * Matrix4::CreateRotationZ(-rotation / 180 * Math::Pi) * Matrix4::CreateTranslation(Vector3(x, y, 0));
	glEnable(GL_TEXTURE_2D);

	shader->set_matrix_uniform("uWorldTransform", world_transform);
	shader->set_matrix_uniform("uViewProj", Matrix4::CreateSimpleViewProj(proj_width, proj_height));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glDisable(GL_TEXTURE_2D);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &tex);

	SDL_FreeSurface(_temp_surface);
	SDL_FreeSurface(surface);
}