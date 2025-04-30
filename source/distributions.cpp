#include "distributions.hpp"

#include "glm/geometric.hpp"

#include "primitive.hpp"
#include "ray.hpp"

#include <algorithm>
#include <cstdlib>
#include <random>
#include <stdexcept>


namespace engine::rand {

Uniform::Uniform()
    : norm(std::normal_distribution<float>(0.f, 1.f)) {}

glm::vec3 Uniform::sample(glm::vec3 x, glm::vec3 n) {
    glm::vec3 sample{
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen())
    };
    sample = glm::normalize(sample);
    if (glm::dot(sample, n) < 0) {
        sample *= -1;
    }
    return sample;
}

float Uniform::pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) {
    if (glm::dot(d, n) < 0) {
        return 0.f;
    }
    return 1.f / (2.f * pi);
}

Cosine::Cosine()
    : norm(std::normal_distribution<float>(0.f, 1.f)) {}

glm::vec3 Cosine::sample(glm::vec3 x, glm::vec3 n) {
    glm::vec3 sample{
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen())
    };
    sample = glm::normalize(sample);
    sample += n;
    return 1.f / glm::length(sample) * sample;
}

float Cosine::pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) {
    return std::max(0.f, glm::dot(d, n) / pi);
}

Light::Light(Shape* obj) : obj(obj) {}

float Light::pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) {
    ray::Ray r;
    r.start = x;
    r.direction = d;

    auto inter = ray::intersection(r, obj);
    if (!inter.has_value()) {
        return 0.f;
    }

    float result = 0.f;
    if (obj->type == PRIMITIVE_TYPE::Box) {
        result += box_pdf(x, d, x + inter->t * d, inter->normal);
    }
    else if (obj->type == PRIMITIVE_TYPE::Ellipsoid) {
        result += ellips_pdf(x, d, x + inter->t * d, inter->normal);
    }
    else if (obj->type == PRIMITIVE_TYPE::Triangle) {
        return triangle_pdf(x, d, x + inter->t * d, inter->normal);
    }

    r.start = x + (inter->t + eps) * d;
    r.direction = d;
    auto next_inter = ray::intersection(r, obj);
    if (!next_inter.has_value()) {
        return result;
    }

    if (obj->type == PRIMITIVE_TYPE::Box) {
        result += box_pdf(x, d, x + (inter->t + next_inter->t + eps) * d, next_inter->normal);
    }
    else if (obj->type == PRIMITIVE_TYPE::Ellipsoid) {
        result += ellips_pdf(x, d, x + (inter->t + next_inter->t + eps) * d, next_inter->normal);
    }
    else if (obj->type == PRIMITIVE_TYPE::Triangle) {
        result += triangle_pdf(x, d, x + (inter->t + next_inter->t + eps) * d, next_inter->normal);
    }
    else {
        throw std::runtime_error("Cannot calculate pdf for Plane");
    }
    return result;
}

glm::vec3 Light::sample(glm::vec3 x, glm::vec3 n) {
    switch (obj->type) {
        case PRIMITIVE_TYPE::Box:
            return box_sample(x, n);
        case PRIMITIVE_TYPE::Ellipsoid:
            return ellips_sample(x, n);
        case PRIMITIVE_TYPE::Triangle:
            return triangle_sample(x, n);    
        default:
            throw std::runtime_error("Cannot generate sample for Plane");    
    }
}

float Light::box_pdf(glm::vec3 x, glm::vec3 d, glm::vec3 inter_point, glm::vec3 inter_norm) {
    auto box = dynamic_cast<Box*>(obj);
    if (box == nullptr) {
        throw std::runtime_error("Failed to cast shape to Box");
    }
    auto numer = glm::dot(x - inter_point, x - inter_point);
    auto denom = 8 * (box->size.y * box->size.z + box->size.x * box->size.z + box->size.x * box->size.y) * std::abs(glm::dot(d, inter_norm));
    return numer / denom;
}

glm::vec3 Light::box_sample(glm::vec3 x, glm::vec3 n) {
    auto box = dynamic_cast<Box*>(obj);
    if (box == nullptr) {
        throw std::runtime_error("Failed to cast shape to Box");
    }

    float weight_x = box->size.y * box->size.z;
    float weight_y = box->size.x * box->size.z;
    float weight_z = box->size.x * box->size.y;
    float weight_sum = weight_x + weight_y + weight_z;

    float u = Rng::get_instance().uniform_01();
        float norm_u = u * weight_sum;
        
        float coin_toss = Rng::get_instance().uniform_01();
        float edge = (coin_toss < 0.5f) ? 1.f : -1.f;
        glm::vec3 point{
            (2 * Rng::get_instance().uniform_01() - 1) * box->size.x,  // U(-1, 1) * size 
            (2 * Rng::get_instance().uniform_01() - 1) * box->size.y, 
            (2 * Rng::get_instance().uniform_01() - 1) * box->size.z
        }; 
        
        if (norm_u < weight_x) {
            point.x = edge * box->size.x; 
        }
        else if (norm_u < weight_x + weight_y) {
            point.y = edge * box->size.y;
        }
        else {
            point.z = edge * box->size.z;
        }

        point = obj->rotation * point;
        point += obj->position;

        return glm::normalize(point - x);
}

float Light::ellips_pdf(glm::vec3 x, glm::vec3 d, glm::vec3 inter_point, glm::vec3 inter_norm) {
    auto ellips = dynamic_cast<Ellipsoid*>(obj);
    if (ellips == nullptr) {
        throw std::runtime_error("Failed to cast shape to Ellipsoid");
    }

    glm::vec3 v = inter_point - ellips->position;
    auto rotation = ellips->rotation * glm::quat{0.0, v.z, v.y, v.z} * 
        glm::quat{ellips->rotation.w, -1 * ellips->rotation.x, -1 * ellips->rotation.y, -1 * ellips->rotation.z};

    glm::vec3 norm = glm::vec3{
        rotation.x / ellips->radius.x,
        rotation.y / ellips->radius.y,
        rotation.z / ellips->radius.z
    };
    
    auto rad = ellips->radius;
    glm::vec3 vec = glm::vec3{norm.x * rad.y * rad.z, rad.x * norm.y * rad.z, rad.x * rad.y * norm.z};
    float pdf = 1.0 / (4 * pi * std::sqrt(glm::dot(vec, vec)));

    return pdf * glm::dot(x - inter_point, x - inter_point) / std::abs(glm::dot(d, inter_norm));
}

glm::vec3 Light::ellips_sample(glm::vec3 x, glm::vec3 n) {
    auto ellips = dynamic_cast<Ellipsoid*>(obj);
    if (ellips == nullptr) {
        throw std::runtime_error("Failed to cast shape to Ellipsoid");
    }

    glm::vec3 point = glm::normalize(glm::vec3(
        Rng::get_instance().normal_01(),
        Rng::get_instance().normal_01(),
        Rng::get_instance().normal_01()
    ));
    point *= ellips->radius;
    point = ellips->rotation * point + ellips->position;

    return glm::normalize(point - x);
}

float Light::triangle_pdf(glm::vec3 x, glm::vec3 d, glm::vec3 inter_point, glm::vec3 inter_norm) {
    auto triangle = dynamic_cast<Triangle*>(obj);
    if (triangle == nullptr) {
        throw std::runtime_error("Failed to cast shape to Triangle");
    }
    float square = 0.5f * glm::length(glm::cross(triangle->vertex_2 - triangle->vertex_1, 
                                                 triangle->vertex_3 - triangle->vertex_1));
    return 1 / square;
}

glm::vec3 Light::triangle_sample(glm::vec3 x, glm::vec3 n) {
    auto triangle = dynamic_cast<Triangle*>(obj);
    if (triangle == nullptr) {
        throw std::runtime_error("Failed to cast shape to Triangle");
    }

    float u = Rng::get_instance().uniform_01();
    float v = Rng::get_instance().uniform_01();
    if (u + v > 1) {
        u = 1 - u;
        v = 1 - v;
    }
    
    glm::vec3 a = triangle->vertex_1;
    glm::vec3 b = triangle->vertex_2;
    glm::vec3 c = triangle->vertex_3;

    glm::vec3 point = a + (b - a) * u + (c - a) * v;
    point = obj->rotation * point + obj->position;
    return glm::normalize(point - x);
}

Mix::Mix(std::vector<Shape*> shapes) {
    size_t finite_obj_count = std::partition(shapes.begin(), shapes.end(), [](const auto obj) {
        return obj->type == PRIMITIVE_TYPE::Box || obj->type == PRIMITIVE_TYPE::Ellipsoid || obj->type == PRIMITIVE_TYPE::Triangle;
    }) - shapes.begin();

    bvh = BVH(shapes, finite_obj_count);
    cosine = std::make_unique<Cosine>();

    for (int i = 0; i < finite_obj_count; ++i) {
        Shape* obj = shapes[i];
        if (obj->type == PRIMITIVE_TYPE::Box) {
            distrs.push_back(std::make_unique<Light>(obj));
        } 
        else if (obj->type == PRIMITIVE_TYPE::Ellipsoid) {
            distrs.push_back(std::make_unique<Light>(obj));
        } 
        else if (obj->type == PRIMITIVE_TYPE::Triangle) {
            distrs.push_back(std::make_unique<Light>(obj));
        }
    }

    if (shapes.size() == 0) {
        only_cosine = true;
    }
}

glm::vec3 Mix::sample(glm::vec3 x, glm::vec3 n) {
    if (only_cosine) {
        return cosine->sample(x, n);
    }   
    auto coin_toss = Rng::get_instance().uniform_01();
    if (coin_toss < 0.5f) {
        return cosine->sample(x, n);
    }
    return distrs[Rng::get_instance().choice(distrs.size())]->sample(x, n);
}

float Mix::pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) {
    float pdf = cosine->pdf(x, n, d);
    for (auto& distr : distrs) {
        pdf += distr->pdf(x, n, d);
    }
    return pdf / static_cast<float>(distrs.size() + 1);
}


float Mix::bvh_pdf(std::uint32_t node_idx, glm::vec3 x, glm::vec3 n, glm::vec3 d) {
    ray::Ray ray;
    ray.start = x;
    ray.direction = d;
    Node node = bvh.nodes[node_idx];

    auto inter = node.aabb.intersect(ray);
    if (!inter.has_value()) {
        return 0.f;
    }
    if (node.left_child == 0) {
        float total_pdf = 0.f;
        for (uint32_t i = node.first_idx; i < node.last_idx; ++i) {
            total_pdf += distrs[i]->pdf(x, n, d);
        }
        return total_pdf;
    }

    return bvh_pdf(node.left_child, x, n, d) + bvh_pdf(node.right_child, x, n, d);
}

} // namespace engine::rand
