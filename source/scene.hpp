#pragma once

#include "glm/vec3.hpp"

#include "primitive.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

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
    glm::vec3 ambient_light;
    Camera camera;
    std::uint32_t ray_depth;
    std::vector<Shape*> primitives;
    std::vector<Light*> lights;

    ~Scene();
};

std::ostream& operator<<(std::ostream& out, const Scene& scene);

} // namespace engine
