#pragma once

#include "scene.hpp"

#include <cstdint>
#include <vector>

namespace engine {

using Image = std::vector<std::uint8_t>;
Image generate_image(const Scene& scene);

} // namespace engine
