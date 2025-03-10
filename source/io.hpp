#pragma once

#include "image.hpp"
#include "scene.hpp"

#include <string>

namespace engine::io {

Scene load_scene(const std::string& a_path);
void write_image(const std::string& a_path, std::uint32_t width, std::uint32_t height, const Image& a_image);

} // namespace engine::io
