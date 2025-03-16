#include "scene.hpp"
#include "primitive.hpp"

#include <stdexcept>

namespace engine {

Scene::~Scene() {
    for (Shape* primitive : primitives) {
        delete primitive;
    }
    for (Light* light : lights) {
        delete light;
    }
}

std::ostream& operator<<(std::ostream& out, const Scene& scene) {
    out << "H: " << scene.height << " W: " << scene.width << '\n'
        << "BG_COLOR (R G B): " << scene.bg_color.r << ' ' << scene.bg_color.g << ' ' << scene.bg_color.b << '\n'
        << "AMBIENT_LIGHT (R G B): " << scene.ambient_light.r << ' ' << scene.ambient_light.g << ' ' << scene.ambient_light.b << "\n\n"
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

    for (auto light : scene.lights) {
        out << "Light:\n"
            << "Intensity: " << light->intensity.r << ' ' << light->intensity.g << ' ' << light->intensity.b << '\n'
            << "Direction: " << light->direction.x << ' ' << light->direction.y << ' ' << light->direction.z << '\n'
            << "Position: " << light->position.x << ' ' << light->position.y << ' ' << light->position.z << '\n'
            << "Attenuation: " << light->attenuation.x << ' ' << light->attenuation.y << ' ' << light->attenuation.z << "\n\n";
    }

    return out;
}

} // namespace engine
