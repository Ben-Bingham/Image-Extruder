#pragma once

#include "Mesh.h"
#include "Image.h"

class ImageExtruder {
public:
	virtual Mesh ExtrudeImage(const Image& image) = 0;
};
