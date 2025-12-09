#include "Sphere.h"
#include <glm/glm.hpp>

Sphere::Sphere(const glm::vec3& c, float r, MaterialType mt, const glm::vec3& color)
    : Object(SPHERE, mt, color), center(c), radius(r) {}

glm::vec3 Sphere::getCenter() const {
    return center;
}

float Sphere::getRadius() const {
    return radius;
}

bool Sphere::intersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& t) const {
    glm::vec3 oc = rayOrigin - center;
    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(oc, rayDirection);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    }

    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2 * a);
    float t2 = (-b + sqrtDisc) / (2 * a);

    if (t1 > 0) {
        t = t1;
        return true;
    }
    if (t2 > 0) {
        t = t2;
        return true;
    }

    return false;
}

glm::vec3 Sphere::getNormal(const glm::vec3& point) const {
    return glm::normalize(point - center);
}
