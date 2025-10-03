#pragma once

#include "ImageExtruder.h"

class AxialGreedy : public ImageExtruder {
public:
	Mesh ExtrudeImage(const Image& image) override;
};