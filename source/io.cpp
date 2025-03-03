#include "io.hpp"
#include "primitive.hpp"

#include <fstream>
#include <sstream>

namespace io {

    Scene load_scene(const std::string& a_path) {
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
                ss >> scene.camera_position.x >> scene.camera_position.y >> scene.camera_position.z;
            }
            else if (command == "CAMERA_RIGHT") {
                ss >> scene.camera_right.x >> scene.camera_right.y >> scene.camera_right.z;
            } 
            else if (command == "CAMERA_UP") {
                ss >> scene.camera_up.x >> scene.camera_up.y >> scene.camera_up.z;
            } 
            else if (command == "CAMERA_FORWARD") {
                ss >> scene.camera_forward.x >> scene.camera_forward.y >> scene.camera_forward.z;
            } 
            else if (command == "CAMERA_FOV_X") {
                ss >> scene.camera_fov_x;
            }
            else if (command == "NEW_PRIMITIVE") {
                std::getline(in, line);
                std::stringstream ss(line);
                ss >> command;
                if (command == "PLANE") {
                    Plane new_plane{};
                    ss >> new_plane.normal.x >> new_plane.normal.y >> new_plane.normal.z;
                    scene.primitives.push_back(new_plane);
                }
                else if (command == "ELLIPSOID") {
                    Ellipsoid new_ellipsoid{};
                    ss >> new_ellipsoid.radius.x >> new_ellipsoid.radius.y >> new_ellipsoid.radius.z;
                    scene.primitives.push_back(new_ellipsoid);
                }
                else if (command == "BOX") {
                    Box new_box{};
                    ss >> new_box.size.x >> new_box.size.y >> new_box.size.z;
                    scene.primitives.push_back(new_box);
                }
            }
            else if  (command == "POSITION") {
                auto& primitive = scene.primitives.back();
                ss >> primitive.position.x >> primitive.position.y >> primitive.position.z;
            }
            else if (command == "ROTATION") {
                auto& primitive = scene.primitives.back();
                ss >> primitive.rotation.x >> primitive.rotation.y >> primitive.rotation.z >> primitive.rotation.w;
            }
            else if (command == "COLOR") {
                auto& primitive = scene.primitives.back();
                ss >> primitive.color.x >> primitive.color.y >> primitive.position.z;
            }
        }
        return scene;
    }

} // namespace io
