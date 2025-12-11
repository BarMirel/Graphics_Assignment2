#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>

enum ObjectType {
    SPHERE,
    PLANE
};

enum MaterialType {
    NORMAL,
    REFLECTIVE,
    TRANSPARENT
};

class Object {
protected:
    ObjectType type;
    MaterialType materialType;
    glm::vec3 color; // Ambient and Diffuse Material color (r, g, b)

public:
    Object(ObjectType t, MaterialType mt, const glm::vec3& c);
    virtual ~Object() = default;

    ObjectType getType() const;
    MaterialType getMaterialType() const;
    void setColor(const glm::vec3& c);

    virtual bool intersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& t) const = 0;
    virtual glm::vec3 getColor(const glm::vec3 hitPoint) const = 0;
    virtual glm::vec3 getNormal(const glm::vec3& point) const = 0;
};

#endif // OBJECT_H
