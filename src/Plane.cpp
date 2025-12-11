#include "Plane.h"
#include <glm/glm.hpp>

Plane::Plane(float a, float b, float c, float d, MaterialType mt, const glm::vec3& color)
    : Object(PLANE, mt, color), a(a), b(b), c(c), d(d) {}

float Plane::getA() const {
    return a;
}

float Plane::getB() const {
    return b;
}

float Plane::getC() const {
    return c;
}

float Plane::getD() const {
    return d;
}

bool Plane::intersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& t) const {
    float denominator = a * rayDirection.x + b * rayDirection.y + c * rayDirection.z;

    if (abs(denominator) < 1e-6) {
        return false; // Ray is parallel to the plane
    }

    float numerator = -(a * rayOrigin.x + b * rayOrigin.y + c * rayOrigin.z + d);
    t = numerator / denominator;

    return t > 0;
}

glm::vec3 Plane::getNormal(const glm::vec3& point) const {
    return glm::normalize(glm::vec3(a, b, c));
}

glm::vec3 Plane::getColor(const glm::vec3 hitPoint) const{
    // Checkerboard pattern
    float scaleParameter = 0.5f;
    float checkerboard = 0;
    if (hitPoint.x < 0) {
        checkerboard += floor((0.5 - hitPoint.x) / scaleParameter);
    }
    else {
        checkerboard += floor(hitPoint.x / scaleParameter);
    }
    if (hitPoint.y < 0) {
        checkerboard += floor((0.5 - hitPoint.y) / scaleParameter);
    }
    else {
        checkerboard += floor(hitPoint.y / scaleParameter);
    }
        checkerboard = (checkerboard * 0.5) - int(checkerboard * 0.5);
        checkerboard *= 2;
    if (checkerboard > 0.5) {
        return 0.5f * color;
    }
        return color;
 }
