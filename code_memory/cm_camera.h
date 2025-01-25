#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <iostream>
#include "cm_class.h"

#include "../FileWatcher.hpp"

class CMCamera : public CMClass {
private:
public:
	CMCamera(unsigned int id);

	Matrix4 get_view_matrix();
	Matrix4 get_proj_matrix();
};
