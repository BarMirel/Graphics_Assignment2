#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "Light.h"

class DirectionalLight : public Light {
private:
    glm::vec3 direction; // Normalized direction vector

public:
    DirectionalLight(const glm::vec3& dir, const glm::vec3& intensity);
    ~DirectionalLight() override = default;

    glm::vec3 getDirection() const;
    void setDirection(const glm::vec3& dir);
};

#endif // DIRECTIONAL_LIGHT_H
