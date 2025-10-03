#include "OrthoCamera.h"

#include <glm/ext/matrix_transform.hpp>

glm::mat4 OrthoCamera::View() const {
    return glm::lookAt(position, position + frontVector, upVector);
}