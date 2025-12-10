#include "IntersectionPoint.h"

IntersectionPoint::IntersectionPoint(const glm::vec3& p, const glm::vec3& n, float tVal, const Object* obj)
    : point(p), normal(n), t(tVal), object(obj) {}

glm::vec3 IntersectionPoint::getPoint() const {
    return point;
}

glm::vec3 IntersectionPoint::getNormal() const {
    return normal;
}

float IntersectionPoint::getT() const {
    return t;
}

const Object* IntersectionPoint::getObject() const {
    return object;
}

void IntersectionPoint::setPoint(const glm::vec3& p) {
    point = p;
}

void IntersectionPoint::setNormal(const glm::vec3& n) {
    normal = n;
}

void IntersectionPoint::setT(float tVal) {
    t = tVal;
}

void IntersectionPoint::setObject(const Object* obj) {
    object = obj;
}
