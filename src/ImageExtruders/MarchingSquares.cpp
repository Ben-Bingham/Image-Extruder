#include "MarchingSquares.h"

#include <glm/glm.hpp>

Mesh MarchingSquares::ExtrudeImage(const Image& image) {
    Mesh mesh{ };

    using Pixel = bool;

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

    if (image.size.y % 2 == 1) {
        std::vector<Pixel> extraRow{ };
        extraRow.resize(image.size.x);
        for (int i = 0; i < image.size.x; ++i) {
            extraRow[i] = false;
        }

        pixels.push_back(extraRow);
    }

    if (image.size.x % 2 == 1) {
        for (int i = 0; i < image.size.y; ++i) {
            pixels[i].push_back(false);
        }
    }

    float left = 0.0f;
    float right = 10.0f;
    float top = 0.0f;
    float bottom = 10.0f;

    float depth = 1.0f;

    float pixelWidth = (right - left) / image.size.x;
    float pixelHeight = (bottom - top) / image.size.y;

    float pwh = pixelWidth / 2.0f;
    float phh = pixelHeight / 2.0f;

    using Cell = unsigned char;
    std::vector<std::vector<Cell>> cells;

    std::vector<glm::vec2> pos2D{ };

    for (int x = 0; x < image.size.x; x += 2) {
        for (int y = 0; y < image.size.y; y += 2) {
            Cell cell = 0;

            bool topLeft = pixels[y][x];
            bool topRight = pixels[y][x + 1];
            bool bottomRight = pixels[y + 1][x + 1];
            bool bottomLeft = pixels[y + 1][x];

            if (topLeft) cell |= 0b00000001;
            if (topRight) cell |= 0b00000010;
            if (bottomRight) cell |= 0b00000100;
            if (bottomLeft) cell |= 0b00001000;

            float TLnormalizedX = x / (float)image.size.x;
            float TLxPosition = TLnormalizedX * (right - left) + left;

            float TLnormalizedY = y / (float)image.size.y;
            float TLyPosition = TLnormalizedY * (bottom - top) + top;

            float BRnormalizedX = (x + 1) / (float)image.size.x;
            float BRxPosition = BRnormalizedX * (right - left) + left;

            float BRnormalizedY = (y + 1) / (float)image.size.y;
            float BRyPosition = BRnormalizedY * (bottom - top) + top;

            glm::vec2 topMiddle{ TLxPosition + pwh, TLyPosition - phh };
            glm::vec2 rightMiddle{ BRxPosition + pwh, BRyPosition - phh };
            glm::vec2 leftMiddle{ TLxPosition - pwh, TLyPosition + phh};
            glm::vec2 bottomMiddle{ BRxPosition - pwh, BRyPosition + phh };

            switch (cell) {
            case 0:
            case 15:
                break;

            case 1:
            case 14:
                pos2D.push_back(leftMiddle);
                pos2D.push_back(topMiddle);
                break;

            case 2:
            case 13:
                pos2D.push_back(topMiddle);
                pos2D.push_back(rightMiddle);
                break;

            case 3:
            case 12:
                pos2D.push_back(leftMiddle);
                pos2D.push_back(rightMiddle);
                break;

            case 4:
            case 11:
                pos2D.push_back(rightMiddle);
                pos2D.push_back(bottomMiddle);
                break;

            case 5:
                pos2D.push_back(topMiddle);
                pos2D.push_back(rightMiddle);

                pos2D.push_back(bottomMiddle);
                pos2D.push_back(leftMiddle);
                break;

            case 6:
            case 9:
                pos2D.push_back(topMiddle);
                pos2D.push_back(bottomMiddle);
                break;

            case 7:
            case 8:
                pos2D.push_back(bottomMiddle);
                pos2D.push_back(leftMiddle);
                break;

            case 10:
                pos2D.push_back(leftMiddle);
                pos2D.push_back(topMiddle);

                pos2D.push_back(rightMiddle);
                pos2D.push_back(bottomMiddle);
                break;
            }
        }
    }

    std::vector<glm::vec3> positions{ };
    std::vector<glm::vec2> uvs{ };
    std::vector<unsigned int> indices{ };
    unsigned int maxIndex = 0;



    std::vector<std::pair<glm::ivec2, glm::ivec2>> squares;

    for (int x = 0; x < image.size.x; ++x) {
        for (int y = 0; y < image.size.y; ++y) {
            if (!pixels[y][x]) continue;

            glm::ivec2 startPoint{ x, y };
            glm::ivec2 endPoint{ x, y };

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