#include "cm_camera.h"

CMCamera::CMCamera(unsigned int id, int width, int height) : CMClass(id, -1, -1, -1, -1), width(width), height(height) {}

Matrix4 CMCamera::get_proj_matrix() {
}