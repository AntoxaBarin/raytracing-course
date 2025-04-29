#include "bvh.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

using std::uint32_t;
using shapes_partition_t = std::pair<float, uint32_t>;

namespace engine {

BVH::BVH(std::vector<Shape*>& shapes, uint32_t shapes_count) {
    root_idx = build_node(shapes, 0, shapes_count);
}

shapes_partition_t best_partition(std::vector<Shape*>& shapes, uint32_t first, uint32_t last) {
    auto count = last - first;
    std::vector<float> prefix_surface_areas(count, 0.f);
    AABB pref_aabb(shapes[first]);
    AABB suf_aabb(shapes[last - 1]);
    
    for (size_t i = 1; i < count; i++) {
        prefix_surface_areas[i] = pref_aabb.surface_area() * i;
        pref_aabb.extend(shapes[first + i]);
        
        const size_t reverse_idx = count - i;
        prefix_surface_areas[reverse_idx] += suf_aabb.surface_area() * i;
        if (reverse_idx > 0) {
            suf_aabb.extend(shapes[first + reverse_idx - 1]);
        }
    }
    
    shapes_partition_t result{prefix_surface_areas[1], first + 1};
    for (size_t i = 2; i < count; i++) {
        if (prefix_surface_areas[i] < result.first) {
            result = {prefix_surface_areas[i], first + i};
        }
    }
    return result;
}

uint32_t BVH::build_node(std::vector<Shape*>& shapes, uint32_t first, uint32_t last) {
    AABB aabb{};
    Node node{};
    node.first_idx = first;
    node.last_idx = last;
    
    if (first < last) {
        aabb = AABB(shapes[first]);
    }
    for (int i = first + 1; i < last; ++i) {
        aabb.extend(shapes[i]);
    }

    node.aabb = aabb;
    uint32_t node_idx = nodes.size();
    nodes.push_back(node);
    if (last - first <= 1) {
        return node_idx;
    }

    auto cmp_X = [](const Shape* lhs, const Shape* rhs) { return lhs->position.x < rhs->position.x; };
    std::sort(shapes.begin() + first, shapes.begin() + last, cmp_X);
    auto partition_X = best_partition(shapes, first, last);
    
    auto cmp_Y = [](const Shape* lhs, const Shape* rhs) { return lhs->position.y < rhs->position.y; };
    std::sort(shapes.begin() + first, shapes.begin() + last, cmp_Y);
    auto partition_Y = best_partition(shapes, first, last);
    
    auto cmp_Z = [](const Shape* lhs, const Shape* rhs) { return lhs->position.z < rhs->position.z; };
    std::sort(shapes.begin() + first, shapes.begin() + last, cmp_Z);
    auto partition_Z = best_partition(shapes, first, last);

    auto best_partition = std::min({partition_X, partition_Y, partition_Z}, 
        [](const shapes_partition_t& lhs, const shapes_partition_t& rhs) {
        return lhs.first < rhs.first;
    });

    nodes[node_idx].left_child = build_node(shapes, first, best_partition.second);
    nodes[node_idx].right_child = build_node(shapes, best_partition.second, last);    
    return node_idx;
}

} // namespace engine
