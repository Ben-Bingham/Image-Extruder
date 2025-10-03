#include "ImageExtruder.h"

Mesh ExtrudeImage(const Image& image) {
    Mesh mesh{ };

    mesh.vertices = std::vector<float>{
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
         0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.5f,
         0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f
    };

    mesh.indices = std::vector<unsigned int>{
        2, 1, 0
    };

    return mesh;
}