#pragma once
#include <glm/glm.hpp>

struct OrthoCamera {
	glm::vec3 position{ 0.0f,  0.0f,  7.5f };
	glm::vec3 frontVector{ 0.0f,  0.0f, -1.0f };
	glm::vec3 upVector{ 0.0f,  1.0f,  0.0f };
	glm::vec3 rightVector{ 1.0f,  0.0f,  0.0f };

	float speed = 5.0f;

	float left = -5.0f;
	float right = 5.0f;
	float bottom = -5.0f;
	float top = 5.0f;

	float nearPlane = 0.05f;
	float farPlane = 100.0f;

	glm::mat4 View() const;
};