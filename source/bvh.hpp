#pragma once

#include "aabb.hpp"

#include <cstdint>
#include <vector>

using std::uint32_t;

namespace engine {

struct Node {
    AABB aabb;
    uint32_t left_child;
    uint32_t right_child;
    uint32_t first_idx;
    uint32_t last_idx;
};

struct BVH {
    BVH() = default;
    BVH(std::vector<Shape*>& shapes, uint32_t shapes_count);

    uint32_t build_node(std::vector<Shape*>& shapes, uint32_t first, uint32_t last);
    std::optional<std::pair<Intersection, int>> intersection(const std::vector<Shape*>& shapes, uint32_t node_idx, 
                                                             ray::Ray& ray, std::optional<float> t) const;    

    uint32_t root_idx;
    std::vector<Node> nodes;
};
    
} // namespace engine
