#pragma once

#include "ImageExtruder.h"

// Attempts to expand in the x acis first, then towards the y axis, starts at (0,0).
class AxialGreedy : public ImageExtruder {
public:
	Mesh ExtrudeImage(const Image& image) override;
};