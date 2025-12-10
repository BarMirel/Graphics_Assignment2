#ifndef INTERSECTION_POINT_H
#define INTERSECTION_POINT_H

#include <glm/glm.hpp>
#include "Object.h"

class IntersectionPoint {
private:
    glm::vec3 point;
    glm::vec3 normal;
    float t; // Parameter along the ray
    const Object* object; // Pointer to the intersected object

public:
    IntersectionPoint(const glm::vec3& p, const glm::vec3& n, float tVal, const Object* obj);
    ~IntersectionPoint() = default;

    glm::vec3 getPoint() const;
    glm::vec3 getNormal() const;
    float getT() const;
    const Object* getObject() const;

    void setPoint(const glm::vec3& p);
    void setNormal(const glm::vec3& n);
    void setT(float tVal);
    void setObject(const Object* obj);
};

#endif // INTERSECTION_POINT_H
