#include "scene.hpp"

std::ostream& operator<<(std::ostream& a_out, const Scene& a_scene) {
    a_out << "H: " << a_scene.height << " W: " << a_scene.width << '\n'
          << "BG_COLOR: " << a_scene.bg_color.x << ' ' << a_scene.bg_color.y << ' ' << a_scene.bg_color.z << '\n'
          << "CAMERA.\nFOV_X: " << a_scene.camera_fov_x << '\n'
          << "POSITION: " << a_scene.camera_position.x << ' ' << a_scene.camera_position.y << ' ' << a_scene.camera_position.z << '\n' 
          << "RIGHT: " << a_scene.camera_right.x << ' ' << a_scene.camera_right.y << ' ' << a_scene.camera_right.z << '\n'
          << "UP: " << a_scene.camera_up.x << ' ' << a_scene.camera_up.y << ' ' << a_scene.camera_up.z << '\n'
          << "FORWARD: " << a_scene.camera_forward.x << ' ' << a_scene.camera_forward.y << ' ' << a_scene.camera_forward.z << '\n';

    return a_out;      
}
