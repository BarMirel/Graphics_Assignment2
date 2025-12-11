#include "Object.h"

Object::Object(ObjectType t, MaterialType mt, const glm::vec3& c, const float s)
    : type(t), materialType(mt), color(c), shininess(s) {}

ObjectType Object::getType() const {
    return type;
}

MaterialType Object::getMaterialType() const {
    return materialType;
}

void Object::setColor(const glm::vec3& c) {
    color = c;
}

float Object::getShininess() const{
    return shininess;
}

void Object::setShininess(const float s) {
    shininess = s;
}
