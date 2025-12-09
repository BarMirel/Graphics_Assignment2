#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"

class Sphere : public Object {
private:
    glm::vec3 center;
    float radius;

public:
    Sphere(const glm::vec3& c, float r, MaterialType mt, const glm::vec3& color);
    ~Sphere() override = default;

    glm::vec3 getCenter() const;
    float getRadius() const;

    bool intersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& t) const override;
    glm::vec3 getNormal(const glm::vec3& point) const override;
};

#endif // SPHERE_H
