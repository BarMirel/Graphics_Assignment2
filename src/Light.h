#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

enum LightType {
    DIRECTIONAL,
    SPOTLIGHT
};

class Light {
protected:
    LightType type;
    glm::vec3 intensity; // (r, g, b)

public:
    Light(LightType t, const glm::vec3& i);
    virtual ~Light() = default;

    LightType getType() const;
    glm::vec3 getIntensity() const;
    void setIntensity(const glm::vec3& i);
};

#endif // LIGHT_H
