#include "ImageExtruder.h"

#include <iostream>

#include <glm/glm.hpp>

Mesh ExtrudeImage(const Image& image) {
    Mesh mesh{ };

    using Pixel = glm::u8vec4;

    std::vector<std::vector<Pixel>> pixels{ };
    //pixels.resize(image.size.x);

    for (int y = 0; y < image.size.y; ++y) {
        pixels.push_back(std::vector<Pixel>{ });
        //std::cout << "Row: " << std::endl;

        for (int x = 0; x < image.size.x * 4; x += 4) {
            glm::u8vec4 pixel{ };
            int r = y * (image.size.x * 4) + x + 0;
            int g = y * (image.size.x * 4) + x + 1;
            int b = y * (image.size.x * 4) + x + 2;
            int a = y * (image.size.x * 4) + x + 3;

            pixel.r = image.data[r];
            pixel.g = image.data[g];
            pixel.b = image.data[b];
            pixel.a = image.data[a];

            //std::cout << (int)pixel.r << ", " << (int)pixel.g << ", " << (int)pixel.b << ", " << (int)pixel.a << std::endl;

            pixels.back().push_back(pixel);
        }
    }

    //for (int x = 0; x < image.size.x; ++x) {
    //    for (int y = 0; y < image.size.y * 4; y += 4) {
    //        glm::u8vec4 pixel{ };
    //        pixel.r = image.data[y * image.size.x + x + 0];
    //        pixel.g = image.data[y * image.size.x + x + 1];
    //        pixel.b = image.data[y * image.size.x + x + 2];
    //        pixel.a = image.data[y * image.size.x + x + 3];

    //        pixels[x].push_back(pixel);
    //    }
    //}

    std::cout << "pixels width: " << pixels[0].size() << std::endl;
    std::cout << "pixels height: " << pixels.size() << std::endl;

    float left = 0.0f;
    float right = 10.0f;
    float top = 0.0f;
    float bottom = 10.0f;

    float depth = 1.0f;

    std::vector<glm::vec3> positions{ };
    std::vector<glm::vec2> uvs{ };
    std::vector<unsigned int> indices{ };
    unsigned int maxIndex = 0;

    float pixelWidth = (right - left) / image.size.x;
    float pixelHeight = (bottom - top) / image.size.y;

    std::cout << "width: " << pixelWidth << std::endl;
    std::cout << "height: " << pixelHeight << std::endl;

    float pwh = pixelWidth / 2.0f;
    float phh = pixelHeight / 2.0f;

    for (int y = 0; y < image.size.y; ++y) {
        for (int x = 0; x < image.size.x; ++x) {
            bool on = pixels[y][x].r > 60; // TODO

            if (!on) continue;

            float normalizedX = x / (float)image.size.x;
            float xPosition = normalizedX * (right - left) + left;

            float normalizedY = y / (float)image.size.y;
            float yPosition = normalizedY * (bottom - top) + top;

            positions.push_back(glm::vec3{ xPosition - pwh, yPosition - phh, 0.0f });
            positions.push_back(glm::vec3{ xPosition + pwh, yPosition - phh, 0.0f });
            positions.push_back(glm::vec3{ xPosition - pwh, yPosition + phh, 0.0f });
            positions.push_back(glm::vec3{ xPosition - pwh, yPosition + phh, 0.0f });
            positions.push_back(glm::vec3{ xPosition + pwh, yPosition - phh, 0.0f });
            positions.push_back(glm::vec3{ xPosition + pwh, yPosition + phh, 0.0f });

            uvs.push_back(glm::vec2{ normalizedX, normalizedY });
            uvs.push_back(glm::vec2{ normalizedX, normalizedY });
            uvs.push_back(glm::vec2{ normalizedX, normalizedY });
            uvs.push_back(glm::vec2{ normalizedX, normalizedY });
            uvs.push_back(glm::vec2{ normalizedX, normalizedY });
            uvs.push_back(glm::vec2{ normalizedX, normalizedY });

            indices.push_back(maxIndex);
            ++maxIndex;
            indices.push_back(maxIndex);
            ++maxIndex;
            indices.push_back(maxIndex);
            ++maxIndex;
            indices.push_back(maxIndex);
            ++maxIndex;
            indices.push_back(maxIndex);
            ++maxIndex;
            indices.push_back(maxIndex);
            ++maxIndex;
        }
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