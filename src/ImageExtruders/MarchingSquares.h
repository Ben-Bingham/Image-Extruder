#pragma once

#include "ImageExtruder.h"

// An implementation of marching squares
class MarchingSquares : public ImageExtruder {
	Mesh ExtrudeImage(const Image& image) override;
};