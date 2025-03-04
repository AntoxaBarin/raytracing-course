#pragma once

#include "image.hpp"
#include "scene.hpp"

#include <string>

namespace io {

Scene load_scene(const std::string& a_path);
void write_image(const std::string& a_path, const Scene& a_scene, const Image& a_image);

} // namespace io
