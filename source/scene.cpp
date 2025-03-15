#include "scene.hpp"
#include "primitive.hpp"

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
          << "BG_COLOR: " << a_scene.bg_color.x << ' ' << a_scene.bg_color.y << ' ' << a_scene.bg_color.z << '\n'
          << "CAMERA.\nFOV_X: " << a_scene.camera.camera_fov_x << '\n'
          << "POSITION: " << a_scene.camera.camera_position.x << ' ' << a_scene.camera.camera_position.y << ' '
          << a_scene.camera.camera_position.z << '\n'
          << "RIGHT: " << a_scene.camera.camera_right.x << ' ' << a_scene.camera.camera_right.y << ' ' << a_scene.camera.camera_right.z
          << '\n'
          << "UP: " << a_scene.camera.camera_up.x << ' ' << a_scene.camera.camera_up.y << ' ' << a_scene.camera.camera_up.z << '\n'
          << "FORWARD: " << a_scene.camera.camera_forward.x << ' ' << a_scene.camera.camera_forward.y << ' '
          << a_scene.camera.camera_forward.z << "\n\n";

    for (auto primitive : a_scene.primitives) {
        a_out << "Primitive: ";
        if (primitive->type == PRIMITIVE_TYPE::Plane) {
            a_out << "Plane\n";
            Plane* plane = dynamic_cast<Plane*>(primitive);
            a_out << plane->normal.x << ' ' << plane->normal.y << ' ' << plane->normal.z << '\n';
        }
        else if (primitive->type == PRIMITIVE_TYPE::Ellipsoid) {
            a_out << "Ellipsoid\n";
            Ellipsoid* ellips = dynamic_cast<Ellipsoid*>(primitive);
            a_out << ellips->radius.x << ' ' << ellips->radius.y << ' ' << ellips->radius.z << '\n';
        }
        else if (primitive->type == PRIMITIVE_TYPE::Box) {
            a_out << "Box\n";
            Box* plane = dynamic_cast<Box*>(primitive);
            a_out << plane->size.x << ' ' << plane->size.y << ' ' << plane->size.z << '\n';
        }
        a_out << "Position: " << primitive->position.x << ' ' << primitive->position.y << ' ' << primitive->position.z << '\n'
              << "Color: " << primitive->color.x << ' ' << primitive->color.y << ' ' << primitive->color.z << "\n\n";
    }

    for (auto light : a_scene.lights) {
        a_out << "Light:\n"
              << light->intensity.r << ' ' << light->intensity.g << ' ' << light->intensity.b << '\n'
              << light->direction.x << ' ' << light->direction.y << ' ' << light->direction.z << '\n'
              << light->position.x << ' ' << light->position.y << ' ' << light->position.z << '\n'
              << light->attenuation.x << ' ' << light->attenuation.y << ' ' << light->attenuation.z << "\n\n";
    }

    return a_out;
}

} // namespace engine
