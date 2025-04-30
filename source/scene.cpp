#include "scene.hpp"

#include "distributions.hpp"
#include "primitive.hpp"

#include <algorithm>
#include <stdexcept>

namespace engine {

void Scene::init_light_distrs() {
    std::vector<Shape*> valid_light_primitives;
    for (auto primitive : primitives) {
        if (primitive->emission != glm::vec3{0.f, 0.f, 0.f}) {
            valid_light_primitives.push_back(primitive);
        }
    }
    distribution = new rand::Mix(valid_light_primitives);
}

void Scene::init_bvh() {
    std::vector<Shape*> shapes;
    auto first_plane_it = std::partition(
        shapes.begin(), 
        shapes.end(),
        [](Shape* obj) { return obj->type != PRIMITIVE_TYPE::Plane; }
    );
    
    first_plane_idx = std::distance(shapes.begin(), first_plane_it);
    auto non_planes = std::vector<Shape*>(shapes.begin(), first_plane_it);
    bvh = BVH(non_planes, non_planes.size());
}

Scene::~Scene() {
    for (Shape* primitive : primitives) {
        delete primitive;
    }
    delete distribution;
}

std::ostream& operator<<(std::ostream& out, const Scene& scene) {
    out << "H: " << scene.height << " W: " << scene.width << '\n'
        << "BG_COLOR (R G B): " << scene.bg_color.r << ' ' << scene.bg_color.g << ' ' << scene.bg_color.b << '\n'
        << "RAY_DEPTH: " << scene.ray_depth << '\n'
        << "CAMERA:\nFOV_X: " << scene.camera.camera_fov_x << '\n'
        << "POSITION: " << scene.camera.camera_position.x << ' ' << scene.camera.camera_position.y << ' ' << scene.camera.camera_position.z
        << '\n'
        << "RIGHT: " << scene.camera.camera_right.x << ' ' << scene.camera.camera_right.y << ' ' << scene.camera.camera_right.z << '\n'
        << "UP: " << scene.camera.camera_up.x << ' ' << scene.camera.camera_up.y << ' ' << scene.camera.camera_up.z << '\n'
        << "FORWARD: " << scene.camera.camera_forward.x << ' ' << scene.camera.camera_forward.y << ' ' << scene.camera.camera_forward.z
        << "\n\n";

    for (auto primitive : scene.primitives) {
        out << "Primitive:\n";
        if (primitive->type == PRIMITIVE_TYPE::Plane) {
            out << "Plane ";
            Plane* plane = dynamic_cast<Plane*>(primitive);
            out << plane->normal.x << ' ' << plane->normal.y << ' ' << plane->normal.z << '\n';
        }
        else if (primitive->type == PRIMITIVE_TYPE::Ellipsoid) {
            out << "Ellipsoid ";
            Ellipsoid* ellips = dynamic_cast<Ellipsoid*>(primitive);
            out << ellips->radius.x << ' ' << ellips->radius.y << ' ' << ellips->radius.z << '\n';
        }
        else if (primitive->type == PRIMITIVE_TYPE::Box) {
            out << "Box ";
            Box* plane = dynamic_cast<Box*>(primitive);
            out << plane->size.x << ' ' << plane->size.y << ' ' << plane->size.z << '\n';
        }
        out << "Position: " << primitive->position.x << ' ' << primitive->position.y << ' ' << primitive->position.z << '\n'
            << "Color: " << primitive->color.x << ' ' << primitive->color.y << ' ' << primitive->color.z << '\n'
            << "Material: ";
        if (primitive->material == MATERIAL_TYPE::Metallic) {
            out << "Metallic\n\n";
        }
        else if (primitive->material == MATERIAL_TYPE::Dielectric) {
            out << "Dielectric\n\n";
        }
        else if (primitive->material == MATERIAL_TYPE::Diffuse) {
            out << "Diffuse\n\n";
        }
        else {
            throw std::runtime_error("Unknown material type.");
        }
    }

    return out;
}

} // namespace engine
