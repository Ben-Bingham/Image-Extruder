#include "SmoothSides.h"

#include <iostream>

#include <glm/glm.hpp>

struct SurroundingPixelInfo {
    bool up, down, left, right, upLeft, upRight, downLeft, downRight;
};
using Pixel = bool;

SurroundingPixelInfo GetSurroundingPixelInfo(int x, int y, glm::ivec2 size, std::vector<std::vector<Pixel>> pixels) {
    SurroundingPixelInfo spi{ };

    spi.up = y - 1 > 0 && pixels[y - 1][x];
    spi.down = y + 1 < size.y && pixels[y + 1][x];

    spi.left = x - 1 > 0 && pixels[y][x - 1];
    spi.right = x + 1 < size.x && pixels[y][x + 1];

    spi.upLeft = y - 1 > 0 && x - 1 > 0 && pixels[y - 1][x - 1];
    spi.upRight = y - 1 > 0 && x + 1 < size.x && pixels[y - 1][x + 1];

    spi.downLeft = y + 1 < size.y && x - 1 > 0 && pixels[y + 1][x - 1];
    spi.downRight = y + 1 < size.y && x + 1 < size.x && pixels[y + 1][x + 1];

    return spi;
}

Mesh SmoothSides::ExtrudeImage(const Image& image) {
    Mesh mesh{ };

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

            if (pixel.r > 60) {
                pixels.back().push_back(false);
            }
            else {
                pixels.back().push_back(true);
            }
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

    std::vector<std::vector<Pixel>> pixelsBackup = pixels;

    for (int x = 0; x < image.size.x; ++x) {
        //std::cout << "done x" << std::endl;
        for (int y = 0; y < image.size.y; ++y) {
            if (!pixels[y][x]) continue;

            glm::ivec2 startPoint{ x, y };
            glm::ivec2 endPoint{ x, y };

            SurroundingPixelInfo spi = GetSurroundingPixelInfo(x, y, image.size, pixelsBackup);

            // -------
            // |o|o|o|
            // -------
            // |x|x|x|
            // -------
            // |x|x|x|
            // -------
            if (!spi.upLeft && !spi.up && !spi.upRight && spi.left && spi.right && spi.downLeft && spi.down && spi.downRight) {
                --startPoint.x;
                ++endPoint.x;

                pixels[y][x - 1] = false;
                pixels[y][x + 1] = false;
                pixels[y][x] = false;

                pixels[y + 1][x - 1] = false;
                pixels[y + 1][x] = false;
                pixels[y + 1][x + 1] = false;
            }

            // -------
            // |x|x|x|
            // -------
            // |x|x|x|
            // -------
            // |o|o|o|
            // -------
            else if (spi.upLeft && spi.up && spi.upRight && spi.left && spi.right && !spi.downLeft && !spi.down  && !spi.downRight) {
                --startPoint.x;
                ++endPoint.x;

                pixels[y][x - 1] = false;
                pixels[y][x + 1] = false;
                pixels[y][x] = false;

                pixels[y - 1][x - 1] = false;
                pixels[y - 1][x] = false;
                pixels[y - 1][x + 1] = false;
            }

            // -------
            // |x|x|o|
            // -------
            // |x|x|o|
            // -------
            // |x|x|o|
            // -------
            else if (spi.upLeft && spi.up && !spi.upRight && spi.left && !spi.right && spi.downLeft && spi.down && !spi.downRight) {
                --startPoint.y;
                ++endPoint.y;

                pixels[y - 1][x] = false;
                pixels[y + 1][x] = false;
                pixels[y][x] = false;

                pixels[y + 1][x - 1] = false;
                pixels[y][x - 1] = false;
                pixels[y - 1][x - 1] = false;
            }

            // -------
            // |o|x|x|
            // -------
            // |o|x|x|
            // -------
            // |o|x|x|
            // -------
            else if (!spi.upLeft && spi.up && spi.upRight && !spi.left && spi.right && !spi.downLeft && spi.down && spi.downRight) {
                --startPoint.y;
                ++endPoint.y;

                pixels[y - 1][x] = false;
                pixels[y + 1][x] = false;
                pixels[y][x] = false;

                pixels[y + 1][x + 1] = false;
                pixels[y][x + 1] = false;
                pixels[y - 1][x + 1] = false;
            }

            // -------
            // |x|x|x|
            // -------
            // |x|x|x|
            // -------
            // |x|x|x|
            // -------
            else if (spi.upLeft && spi.up && spi.upRight && spi.left && spi.right && spi.downLeft && spi.down && spi.downRight) {
                pixels[y - 1][x] = false;
                pixels[y + 1][x] = false;
                pixels[y][x] = false;

                pixels[y][x + 1] = false;
                pixels[y][x - 1] = false;

                pixels[y - 1][x - 1] = false;
                pixels[y + 1][x + 1] = false;

                pixels[y - 1][x + 1] = false;
                pixels[y + 1][x - 1] = false;

                continue;
            }

            // -------
            // |o|o|o|
            // -------
            // |o|x|x|
            // -------
            // |o|x|x|
            // -------
            else if (!spi.upLeft && !spi.up && !spi.upRight && !spi.left && spi.right && !spi.downLeft && spi.down && spi.downRight) {
                pixels[y][x] = false;

                pixels[y][x + 1] = false;
                pixels[y + 1][x] = false;
                pixels[y + 1][x + 1] = false;

                ++endPoint.x;
                ++endPoint.y;
            }

            // -------
            // |o|o|o|
            // -------
            // |x|x|o|
            // -------
            // |x|x|o|
            // -------
            else if (!spi.upLeft && !spi.up && !spi.upRight && spi.left && !spi.right && spi.downLeft && spi.down && !spi.downRight) {
                pixels[y][x] = false;

                pixels[y][x - 1] = false;
                pixels[y + 1][x] = false;
                pixels[y + 1][x - 1] = false;

                --startPoint.x;

                ++endPoint.y;
            }

            // -------
            // |o|x|x|
            // -------
            // |o|x|x|
            // -------
            // |o|o|o|
            // -------
            else if (!spi.upLeft && spi.up && spi.upRight && !spi.left && spi.right && !spi.downLeft && !spi.down && !spi.downRight) {
                pixels[y][x] = false;

                pixels[y][x + 1] = false;
                pixels[y - 1][x] = false;
                pixels[y - 1][x + 1] = false;

                --startPoint.y;

                ++endPoint.x;
            }
            // -------
            // |x|x|o|
            // -------
            // |x|x|o|
            // -------
            // |o|o|o|
            // -------
            else if (spi.upLeft && spi.up && !spi.upRight && spi.left && !spi.right && !spi.downLeft && !spi.down && !spi.downRight) {
                pixels[y][x] = false;

                pixels[y][x - 1] = false;
                pixels[y - 1][x] = false;
                pixels[y - 1][x - 1] = false;

                --startPoint.x;
                --startPoint.y;
            }

            // -------
            // |x|o|o|
            // -------
            // |x|x|x|
            // -------
            // |x|x|x|
            // -------
            else if (spi.upLeft && !spi.up && !spi.upRight && spi.left && spi.right && spi.downLeft && spi.down && spi.downRight) {
                pixels[y][x - 1] = false;
                pixels[y][x + 1] = false;
                pixels[y][x] = false;

                pixels[y - 1][x - 1] = false;
                pixels[y - 1][x] = false;
                pixels[y - 1][x + 1] = false;

                --startPoint.x;
                --startPoint.y;

                ++endPoint.x;
            }

            else {
                std::cout << "-------" << std::endl;
                std::cout << "|" << spi.upLeft << "|" << spi.up << "|" << spi.upRight << "|" << std::endl;
                std::cout << "-------" << std::endl;
                std::cout << "|" << spi.left << "|" << true << "|" << spi.right << "|" << std::endl;
                std::cout << "-------" << std::endl;
                std::cout << "|" << spi.downLeft << "|" << spi.down << "|" << spi.downRight << "|" << std::endl;
                std::cout << "-------" << std::endl;

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