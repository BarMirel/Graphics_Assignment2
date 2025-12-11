#ifndef PLANE_H
#define PLANE_H

#include "Object.h"

class Plane : public Object {
private:
    float a, b, c, d; // Plane equation: ax + by + cz + d = 0

public:
    Plane(float a, float b, float c, float d, MaterialType mt, const glm::vec3& color, const float shininess);
    ~Plane() override = default;

    float getA() const;
    float getB() const;
    float getC() const;
    float getD() const;

    bool intersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& t) const override;
    glm::vec3 getColor(const glm::vec3 hitPoint) const override;
    glm::vec3 getNormal(const glm::vec3& point) const override;
};

#endif // PLANE_H
