#include "io.hpp"
#include "glm/geometric.hpp"
#include "primitive.hpp"

#include <fstream>
#include <ios>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace io {

Scene load_scene(const std::string& a_path)
{
    std::ifstream in(a_path);
    if (!in.is_open()) {
        throw std::runtime_error("Bad path to scene file.");
    }
    Scene scene;
    std::string line;

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string command;

        ss >> command;
        if (command == "DIMENSIONS") {
            ss >> scene.width >> scene.height;
        }
        else if (command == "BG_COLOR") {
            ss >> scene.bg_color.x >> scene.bg_color.y >> scene.bg_color.z;
        }
        else if (command == "CAMERA_POSITION") {
            ss >> scene.camera.camera_position.x >> scene.camera.camera_position.y >> scene.camera.camera_position.z;
        }
        else if (command == "CAMERA_RIGHT") {
            ss >> scene.camera.camera_right.x >> scene.camera.camera_right.y >> scene.camera.camera_right.z;
            scene.camera.camera_right = glm::normalize(scene.camera.camera_right);
        }
        else if (command == "CAMERA_UP") {
            ss >> scene.camera.camera_up.x >> scene.camera.camera_up.y >> scene.camera.camera_up.z;
            scene.camera.camera_up = glm::normalize(scene.camera.camera_up);
        }
        else if (command == "CAMERA_FORWARD") {
            ss >> scene.camera.camera_forward.x >> scene.camera.camera_forward.y >> scene.camera.camera_forward.z;
            scene.camera.camera_forward = glm::normalize(scene.camera.camera_forward);
        }
        else if (command == "CAMERA_FOV_X") {
            ss >> scene.camera.camera_fov_x;
        }
        else if (command == "NEW_PRIMITIVE") {
            std::getline(in, line);
            std::stringstream ss(line);
            ss >> command;
            if (command == "PLANE") {
                Plane* new_plane = new Plane();
                ss >> new_plane->normal.x >> new_plane->normal.y >> new_plane->normal.z;
                new_plane->normal = glm::normalize(new_plane->normal);
                scene.primitives.push_back(new_plane);
            }
            else if (command == "ELLIPSOID") {
                Ellipsoid* new_ellipsoid = new Ellipsoid();
                ss >> new_ellipsoid->radius.x >> new_ellipsoid->radius.y >> new_ellipsoid->radius.z;
                scene.primitives.push_back(new_ellipsoid);
            }
            else if (command == "BOX") {
                Box* new_box = new Box();
                ss >> new_box->size.x >> new_box->size.y >> new_box->size.z;
                scene.primitives.push_back(new_box);
            }
        }
        else if (command == "POSITION") {
            auto primitive = scene.primitives.back();
            ss >> primitive->position.x >> primitive->position.y >> primitive->position.z;
        }
        else if (command == "ROTATION") {
            auto primitive = scene.primitives.back();
            ss >> primitive->rotation.x >> primitive->rotation.y >> primitive->rotation.z >> primitive->rotation.w;
        }
        else if (command == "COLOR") {
            auto primitive = scene.primitives.back();
            ss >> primitive->color.x >> primitive->color.y >> primitive->color.z;
        }
    }
    return scene;
}

void write_image(const std::string& a_path, std::uint32_t width, std::uint32_t height, const Image& a_image)
{
    std::ofstream out(a_path, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Bad path to image file.");
    }
    out << "P6\n" << width << ' ' << height << "\n255\n";
    out.write(reinterpret_cast<const char*>(a_image.data()), width * height * 3);
    out.close();
}

} // namespace io
