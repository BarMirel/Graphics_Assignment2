#include "Light.h"

Light::Light(LightType t, const glm::vec3& i)
    : type(t), intensity(i) {}

LightType Light::getType() const {
    return type;
}

glm::vec3 Light::getIntensity() const {
    return intensity;
}

void Light::setIntensity(const glm::vec3& i) {
    intensity = i;
}
