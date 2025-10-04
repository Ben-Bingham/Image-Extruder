#pragma once

#include "ImageExtruder.h"

// Selects pixels at random, then tries to expand outward in a greedy way
class RandomGreedy : public ImageExtruder {
	Mesh ExtrudeImage(const Image& image) override;
};