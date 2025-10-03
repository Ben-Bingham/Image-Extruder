#include "ImageExtruder.h"

#include <glm/glm.hpp>

Mesh ExtrudeImage(const Image& image) {
    Mesh mesh{ };

    using Pixel = glm::u8vec4;

    std::vector<Pixel> pixels{ };
    pixels.resize(image.size.x * image.size.y);

    for (int i = 0; i < image.size.x * image.size.y; i += 4) {
        pixels[i].r = image.data[i + 0];
        pixels[i].g = image.data[i + 1];
        pixels[i].b = image.data[i + 2];
        pixels[i].a = image.data[i + 3];
    }

    float left = 0.0f;
    float right = 10.0f;
    float top = 0.0f;
    float bottom = 10.0f;

    float depth = 1.0f;

    std::vector<glm::vec3> positions{ };
    std::vector<glm::vec2> uvs{ };
    std::vector<unsigned int> indices{ };
    unsigned int maxIndex = 0;
    for (int i = 0; i < pixels.size(); ++i) {
        bool on = pixels[i].r > 60; // TODO

        if (!on) continue;

        float x = std::floor((float)i / (float)image.size.x);
        float y = i % image.size.y;

        float normalizedX = x / (float)image.size.y;
        float xPosition = normalizedX * (right - left) + left;

        float normalizedY = y / (float)image.size.x;
        float yPosition = normalizedY * (bottom - top) + top;

        positions.push_back(glm::vec3{ xPosition, yPosition, 0.0f });
        positions.push_back(glm::vec3{ xPosition, yPosition, depth });

        uvs.push_back(glm::vec2{ normalizedX, normalizedY });
        uvs.push_back(glm::vec2{ normalizedX, normalizedY });

        indices.push_back(maxIndex);
        ++maxIndex;
        indices.push_back(maxIndex);
        ++maxIndex;
    }

    std::vector<float> vertices{ };
    vertices.reserve(indices.size() * 7);

    for (int i = 0; i < indices.size(); ++i) {
        vertices.push_back(positions[i].x);
        vertices.push_back(positions[i].y);
        vertices.push_back(positions[i].z);

        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        vertices.push_back(uvs[i].x);
        vertices.push_back(uvs[i].y);
    }

    mesh.vertices = vertices;
    mesh.indices = indices;

    return mesh;
}