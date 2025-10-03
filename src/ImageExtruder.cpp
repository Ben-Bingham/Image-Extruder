#include "ImageExtruder.h"

#include <iostream>

#include <glm/glm.hpp>

Mesh ExtrudeImage(const Image& image) {
    Mesh mesh{ };

    using Pixel = glm::u8vec4;

    std::vector<std::vector<Pixel>> pixels{ };

    for (int y = 0; y < image.size.y; ++y) {
        pixels.push_back(std::vector<Pixel>{ });

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

            pixels.back().push_back(pixel);
        }
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

    float pixelWidth = (right - left) / image.size.x;
    float pixelHeight = (bottom - top) / image.size.y;

    float pwh = pixelWidth / 2.0f;
    float phh = pixelHeight / 2.0f;

    std::vector<std::pair<glm::ivec2, glm::ivec2>> squares;

    for (int y = 0; y < image.size.y; ++y) {
        for (int x = 0; x < image.size.x; ++x) {
            if (pixels[y][x].r >= 60) continue;

            glm::ivec2 startPoint{ x, y };
            glm::ivec2 endPoint{ x, y };

            pixels[y][x].r = 255;

            while (endPoint.x + 1 < image.size.x && pixels[y][endPoint.x + 1].r < 60) {
                pixels[y][endPoint.x + 1].r = 255;
                ++endPoint.x;
            }

            while (endPoint.y + 1 < image.size.y) {
                bool canExtendDown = true;
                for (int i = startPoint.x; i < endPoint.x + 1; ++i) {
                    if (pixels[endPoint.y + 1][i].r >= 60) {
                        canExtendDown = false;
                        break;
                    }
                }

                if (canExtendDown) {
                    for (int i = startPoint.x; i < endPoint.x + 1; ++i) {
                        pixels[endPoint.y + 1][i].r = 255;
                    }
                    ++endPoint.y;
                }
                else {
                    break;
                }
            }

            squares.push_back({ startPoint, endPoint });
        }
    }

    for (auto& square : squares) {
        // TL = Top Left
        // BR = Bottom Right

        float TLnormalizedX = square.first.x / (float)image.size.x;
        float TLxPosition = TLnormalizedX * (right - left) + left;

        float TLnormalizedY = square.first.y / (float)image.size.y;
        float TLyPosition = TLnormalizedY * (bottom - top) + top;

        float BRnormalizedX = square.second.x / (float)image.size.x;
        float BRxPosition = BRnormalizedX * (right - left) + left;

        float BRnormalizedY = square.second.y / (float)image.size.y;
        float BRyPosition = BRnormalizedY * (bottom - top) + top;

        positions.push_back(glm::vec3{ TLxPosition - pwh, TLyPosition - phh, 0.0f });
        positions.push_back(glm::vec3{ BRxPosition + pwh, TLyPosition - phh, 0.0f });
        positions.push_back(glm::vec3{ TLxPosition - pwh, BRyPosition + phh, 0.0f });
        positions.push_back(glm::vec3{ TLxPosition - pwh, BRyPosition + phh, 0.0f });
        positions.push_back(glm::vec3{ BRxPosition + pwh, TLyPosition - phh, 0.0f });
        positions.push_back(glm::vec3{ BRxPosition + pwh, BRyPosition + phh, 0.0f });

        uvs.push_back(glm::vec2{ TLnormalizedX, TLnormalizedY });
        uvs.push_back(glm::vec2{ BRnormalizedX, TLnormalizedY });
        uvs.push_back(glm::vec2{ TLnormalizedX, BRnormalizedY });
        uvs.push_back(glm::vec2{ TLnormalizedX, BRnormalizedY });
        uvs.push_back(glm::vec2{ BRnormalizedX, TLnormalizedY });
        uvs.push_back(glm::vec2{ BRnormalizedX, BRnormalizedY });

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