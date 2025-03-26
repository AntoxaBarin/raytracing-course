#pragma once

#include "image.hpp"
#include "scene.hpp"

#include <string>

namespace engine::io {

Scene load_scene(const std::string& path);
void write_image(const std::string& path, std::uint32_t width, std::uint32_t height, const Image& image);

} // namespace engine::io
