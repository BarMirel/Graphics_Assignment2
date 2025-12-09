#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3& dir, const glm::vec3& intensity)
    : Light(DIRECTIONAL, intensity), direction(glm::normalize(dir)) {}

glm::vec3 DirectionalLight::getDirection() const {
    return direction;
}

void DirectionalLight::setDirection(const glm::vec3& dir) {
    direction = glm::normalize(dir);
}
