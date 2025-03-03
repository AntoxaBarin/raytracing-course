#pragma once

#include "scene.hpp"

#include <string>

namespace io {

Scene load_scene(std::string path);
void generate_image(std::string path);

} // namespace io
