#pragma once

#include "ImageExtruder.h"

class SmoothSides : public ImageExtruder {
	Mesh ExtrudeImage(const Image& image) override;
};