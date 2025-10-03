#include "Image.h"

#include <iostream>

Image::Image(const std::string& path, bool flip) {
    int nrChannels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(path.c_str(), &size.x, &size.y, &nrChannels, 4);

    m_Data.resize(4 * size.x * size.y * sizeof(unsigned char));

    if (data) {
        std::memcpy(m_Data.data(), data, 4 * size.x * size.y * sizeof(unsigned char));
    }
    else {
        std::cout << "ERROR: Failed to load image: " << path << std::endl;
    }
    stbi_image_free(data);
}