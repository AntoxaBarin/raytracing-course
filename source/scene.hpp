#pragma once

#include "distributions.hpp"
#include "glm/vec3.hpp"

#include "primitive.hpp"

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>

namespace engine {

struct Camera {
    float camera_fov_x;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;
};

struct Scene {
    std::uint32_t height;
    std::uint32_t width;
    glm::vec3 bg_color;
    Camera camera;
    std::uint32_t ray_depth;
    std::uint32_t samples;
    std::vector<Shape*> primitives;
    rand::Mix* distribution;

    void init_light_distrs();

    ~Scene();
};

std::ostream& operator<<(std::ostream& out, const Scene& scene);

} // namespace engine
