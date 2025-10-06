#include "Image.h"

#include <iostream>

Image::Image(const std::string& path, bool flip) {
    int x, y, nrChannels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* d = stbi_load(path.c_str(), &x, &y, &nrChannels, 4);

    size.x = x;
    size.y = y;


    if (d) {
        data.resize(4 * size.x * size.y * sizeof(unsigned char));

        std::memcpy(data.data(), d, 4 * size.x * size.y * sizeof(unsigned char));
    }
    else {
        std::cout << "ERROR: Failed to load image: " << path << std::endl;
    }
    stbi_image_free(d);
}