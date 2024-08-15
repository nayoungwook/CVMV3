#pragma once

#include <SDL.h>
#include <gl/glew.h>
#include <iostream>
#include "cm_function.h"
#include "cm_class.h"
#include "cm_scene.h"
#include "memory.h"
#include "../cvm.h"
#include "../function_frame.h"
#include "../sys_util.h"
#include "../sys_file.h"
#include "cm_image.h"

#include "../FileWatcher.hpp"

class CMCamera : public CMClass {
private:
public:
	CMCamera();
};
