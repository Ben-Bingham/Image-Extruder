#pragma once

#include <vector>
#include <string>

#include <glm/vec2.hpp>

#include <stb_image.h>

class Image {
public:
	Image(const std::string& path, bool flip = true);

	glm::ivec2 size{ 0, 0 };
	std::vector<unsigned char> data;
};
