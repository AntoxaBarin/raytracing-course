#pragma once

#include "scene.hpp"

#include <array>
#include <cstdint>
#include <vector>

using Image = std::vector<std::uint8_t>;
Image generate_image(const Scene& a_scene);
