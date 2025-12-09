#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Light.h"

class Spotlight : public Light {
private:
    glm::vec3 position; // (x, y, z)
    glm::vec3 direction; // (r, g, b) - normalized direction
    float cutoffCosine; // cosine of the cutoff angle

public:
    Spotlight(const glm::vec3& pos, const glm::vec3& dir, float cutoffCos, const glm::vec3& intensity);
    ~Spotlight() override = default;

    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;
    float getCutoffCosine() const;

    void setPosition(const glm::vec3& pos);
    void setDirection(const glm::vec3& dir);
    void setCutoffCosine(float cutoffCos);
};

#endif // SPOTLIGHT_H
