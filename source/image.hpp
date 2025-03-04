#pragma once

#include "scene.hpp"

#include <cstdint>
#include <vector>

using Image = std::vector<std::uint8_t>;
Image generate_image(const Scene& a_scene);
