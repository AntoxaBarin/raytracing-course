#pragma once

#include <numbers>
#include <random>
#include <memory>

#include "glm/ext/vector_float3.hpp"

#include "bvh.hpp"
#include "primitive.hpp"

namespace engine::rand {

constexpr float pi = std::numbers::pi_v<float>;
constexpr float eps = 1e-4;

class Rng {
public:
    static Rng& get_instance() {
        static Rng instance;
        return instance;
    }

    std::minstd_rand& gen() {
        thread_local std::random_device rd;
        thread_local std::minstd_rand gen(rd());
        return gen;
    }

    float uniform_01() {
        thread_local std::uniform_real_distribution<float> uniform {0.f, 1.f};
        return uniform(gen());
    }

    float normal_01() {
        thread_local std::normal_distribution<float> normal {0.f, 1.f};
        return normal(gen());
    }

    int choice(std::size_t size) {
        auto choice = static_cast<int>(size * uniform_01());
        if (choice >= size && size != 0) {
            return choice - 1;
        }
        return choice; 
    }

private:
    Rng() = default;
    Rng(const Rng&) = delete;
    Rng& operator=(const Rng&) = delete;
};

class IDistribution {
public:
    virtual ~IDistribution() = default;
    virtual glm::vec3 sample(glm::vec3 x, glm::vec3 n) = 0;
    virtual float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) = 0;
};

class Uniform : public IDistribution {
public:
    Uniform();

    glm::vec3 sample(glm::vec3 x, glm::vec3 n) final;
    float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) final;

private:
    std::normal_distribution<float> norm;
};

class Cosine : public IDistribution {
public:
    Cosine();

    glm::vec3 sample(glm::vec3 x, glm::vec3 n) final;
    float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) final;

private:
    std::normal_distribution<float> norm;
};

class Light : public IDistribution {
public:
    Light(Shape* obj);

    float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) final;
    glm::vec3 sample(glm::vec3 x, glm::vec3 n) final;

    float box_pdf(glm::vec3 x, glm::vec3 d, glm::vec3 inter_point, glm::vec3 inter_norm);
    glm::vec3 box_sample(glm::vec3 x, glm::vec3 n);
    
    float ellips_pdf(glm::vec3 x, glm::vec3 d, glm::vec3 inter_point, glm::vec3 inter_norm);
    glm::vec3 ellips_sample(glm::vec3 x, glm::vec3 n);

    float triangle_pdf(glm::vec3 x, glm::vec3 d, glm::vec3 inter_point, glm::vec3 inter_norm);
    glm::vec3 triangle_sample(glm::vec3 x, glm::vec3 n);

private:
    Shape* obj;    
};

class Mix : public IDistribution {
public:
    Mix(std::vector<Shape*> shapes);

    float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) final;
    glm::vec3 sample(glm::vec3 x, glm::vec3 n) final;

private:
    float bvh_pdf(std::uint32_t node_idx, glm::vec3 x, glm::vec3 n, glm::vec3 d);

    std::unique_ptr<Cosine> cosine;
    std::vector<std::unique_ptr<Light>> distrs;
    BVH bvh;
    bool only_cosine = false;
};


} // namespace engine::rand
