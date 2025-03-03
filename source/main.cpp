#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./engine <path-to-scene> <path-to-image>\n";
        return 1;
    }

    return 0;
}
