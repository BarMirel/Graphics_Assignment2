#include "Spotlight.h"

Spotlight::Spotlight(const glm::vec3& pos, const glm::vec3& dir, float cutoffCos, const glm::vec3& intensity)
    : Light(SPOTLIGHT, intensity), position(pos), direction(glm::normalize(dir)), cutoffCosine(cutoffCos) {}

glm::vec3 Spotlight::getPosition() const {
    return position;
}

glm::vec3 Spotlight::getDirection() const {
    return direction;
}

float Spotlight::getCutoffCosine() const {
    return cutoffCosine;
}

void Spotlight::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Spotlight::setDirection(const glm::vec3& dir) {
    direction = glm::normalize(dir);
}

void Spotlight::setCutoffCosine(float cutoffCos) {
    cutoffCosine = cutoffCos;
}
