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

std::ostream& operator<<(std::ostream& a_out, const Scene& a_scene) {
    a_out << "H: " << a_scene.height << " W: " << a_scene.width << '\n'
          << "BG_COLOR (R G B): " << a_scene.bg_color.r << ' ' << a_scene.bg_color.g << ' ' << a_scene.bg_color.b << '\n'
          << "AMBIENT_LIGHT (R G B): " << a_scene.ambient_light.r << ' ' << a_scene.ambient_light.g << ' ' << a_scene.ambient_light.b
          << "\n\n"
          << "RAY_DEPTH: " << a_scene.ray_depth << '\n'
          << "CAMERA:\nFOV_X: " << a_scene.camera.camera_fov_x << '\n'
          << "POSITION: " << a_scene.camera.camera_position.x << ' ' << a_scene.camera.camera_position.y << ' '
          << a_scene.camera.camera_position.z << '\n'
          << "RIGHT: " << a_scene.camera.camera_right.x << ' ' << a_scene.camera.camera_right.y << ' ' << a_scene.camera.camera_right.z
          << '\n'
          << "UP: " << a_scene.camera.camera_up.x << ' ' << a_scene.camera.camera_up.y << ' ' << a_scene.camera.camera_up.z << '\n'
          << "FORWARD: " << a_scene.camera.camera_forward.x << ' ' << a_scene.camera.camera_forward.y << ' '
          << a_scene.camera.camera_forward.z << "\n\n";

    for (auto primitive : a_scene.primitives) {
        a_out << "Primitive:\n";
        if (primitive->type == PRIMITIVE_TYPE::Plane) {
            a_out << "Plane ";
            Plane* plane = dynamic_cast<Plane*>(primitive);
            a_out << plane->normal.x << ' ' << plane->normal.y << ' ' << plane->normal.z << '\n';
        }
        else if (primitive->type == PRIMITIVE_TYPE::Ellipsoid) {
            a_out << "Ellipsoid ";
            Ellipsoid* ellips = dynamic_cast<Ellipsoid*>(primitive);
            a_out << ellips->radius.x << ' ' << ellips->radius.y << ' ' << ellips->radius.z << '\n';
        }
        else if (primitive->type == PRIMITIVE_TYPE::Box) {
            a_out << "Box ";
            Box* plane = dynamic_cast<Box*>(primitive);
            a_out << plane->size.x << ' ' << plane->size.y << ' ' << plane->size.z << '\n';
        }
        a_out << "Position: " << primitive->position.x << ' ' << primitive->position.y << ' ' << primitive->position.z << '\n'
              << "Color: " << primitive->color.x << ' ' << primitive->color.y << ' ' << primitive->color.z << '\n'
              << "Material: ";
        if (primitive->material == MATERIAL_TYPE::Metallic) {
            a_out << "Metallic\n\n";
        }
        else if (primitive->material == MATERIAL_TYPE::Dielectric) {
            a_out << "Dielectric\n\n";
        }
        else if (primitive->material == MATERIAL_TYPE::Diffuse) {
            a_out << "Diffuse\n\n";
        }
        else {
            throw std::runtime_error("Unknown material type.");
        }
    }

    for (auto light : a_scene.lights) {
        a_out << "Light:\n"
              << "Intensity: " << light->intensity.r << ' ' << light->intensity.g << ' ' << light->intensity.b << '\n'
              << "Direction: " << light->direction.x << ' ' << light->direction.y << ' ' << light->direction.z << '\n'
              << "Position: " << light->position.x << ' ' << light->position.y << ' ' << light->position.z << '\n'
              << "Attenuation: " << light->attenuation.x << ' ' << light->attenuation.y << ' ' << light->attenuation.z << "\n\n";
    }

    return a_out;
}

} // namespace engine
