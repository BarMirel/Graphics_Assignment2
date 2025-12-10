#include "Ray.h"

Ray::Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(glm::normalize(d)) {}

glm::vec3 Ray::getOrigin() const {
    return origin;
}

glm::vec3 Ray::getDirection() const {
    return direction;
}

void Ray::setOrigin(const glm::vec3& o) {
    origin = o;
}

void Ray::setDirection(const glm::vec3& d) {
    direction = glm::normalize(d);
}

glm::vec3 Ray::pointAt(float t) const {
    return origin + t * direction;
}
