#pragma once

#include "glm/vec3.hpp"

#include "primitive.hpp"

#include <cstdint>
#include <variant>
#include <vector>

struct Scene {
    std::uint32_t height;
    std::uint32_t width;
    float camera_fov_x;

    glm::vec3 bg_color;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;

    std::vector<shape> primitives;
};
