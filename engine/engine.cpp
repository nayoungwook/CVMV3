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

int nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return round(pow(2, ceil(logbase2)));
}

void render_text(SDL_Renderer* renderer, CMShader* shader,
	std::string const& str, float x, float y, float _r, float _g, float _b, float _a, float rotation, int proj_width, int proj_height) {

}