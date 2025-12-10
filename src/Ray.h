#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class Ray {
private:
    glm::vec3 origin;
    glm::vec3 direction; // Normalized direction vector

public:
    Ray(const glm::vec3& o, const glm::vec3& d);
    ~Ray() = default;

    glm::vec3 getOrigin() const;
    glm::vec3 getDirection() const;

    void setOrigin(const glm::vec3& o);
    void setDirection(const glm::vec3& d);

    glm::vec3 pointAt(float t) const; // Get point along ray at parameter t
};

#endif // RAY_H
