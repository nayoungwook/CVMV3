#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <iostream>
#include "cm_class.h"
#include "../Math.h"

#include "../FileWatcher.hpp"

class CMCamera : public CMClass {
private:
	int width, height;
public:
	CMCamera(unsigned int id, int width, int height);

	Matrix4 proj_matrix, view_matrix;
	Matrix4 get_proj_matrix();
};
