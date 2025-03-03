#pragma once

#include "scene.hpp"

#include <string>

namespace io {

Scene load_scene(const std::string& a_path);
void generate_image(const std::string& a_path);

} // namespace io
